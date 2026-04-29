#!/usr/bin/env python3
"""Update the DBeaver decryption key across all source flavours.

Fetches the current key from DBeaver's upstream source (or accepts a hex
string on the command line for offline use), then rewrites:

- dbeaver-creds (Bash)
- dbeaver-creds.bat (Batch)
- DBeaverCreds/DBeaverCreds.psm1 (PowerShell)
- src/api.c (C)
- tests/test_backend.c (C, both key and recomputed test ciphertext)
- tests/test_api.c (C, recomputed test ciphertext)

The test ciphertexts are recomputed via the cryptography library so the
cmocka suite stays in sync with the rotated key.
"""
from __future__ import annotations

from pathlib import Path
from urllib.request import urlopen
import argparse
import re
import sys

from cryptography.hazmat.primitives import padding
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes

DBEAVER_KEY_URL = ('https://raw.githubusercontent.com/dbeaver/dbeaver/devel/'
                   'plugins/org.jkiss.dbeaver.registry/src/org/jkiss/dbeaver/'
                   'registry/BaseProjectImpl.java')
KEY_LINE_PATTERN = re.compile(r'LOCAL_KEY_CACHE\s*=\s*\{(?P<bytes>[^}]+)\}')
KEY_BYTES = 16
KEY_HEX_CHARS = 32
CIPHER_HEX_CHARS = 96
TEST_PLAINTEXT = b'A' * KEY_BYTES + b'{"hello":"world"}'


def fetch_key() -> str:
    """Fetch the upstream DBeaver Java source and extract the key as 32 hex chars.

    Returns
    -------
    str
        The 32-character lowercase hex representation of the AES key.

    Raises
    ------
    RuntimeError
        If the upstream source does not contain the expected key constant or
        the byte count is wrong.
    """
    with urlopen(DBEAVER_KEY_URL) as response:
        source = response.read().decode('utf-8')
    match = KEY_LINE_PATTERN.search(source)
    if not match:
        msg = 'Could not locate LOCAL_KEY_CACHE in upstream source.'
        raise RuntimeError(msg)
    signed = [int(token.strip()) for token in match.group('bytes').split(',') if token.strip()]
    if len(signed) != KEY_BYTES:
        msg = f'Expected {KEY_BYTES} bytes from upstream; got {len(signed)}.'
        raise RuntimeError(msg)
    return ''.join(f'{b & 0xff:02x}' for b in signed)


def encrypt_test_plaintext(key_hex: str) -> str:
    """Encrypt the fixture plaintext with AES-128-CBC under the given key.

    Returns
    -------
    str
        Hex-encoded ciphertext (96 hex chars = 48 bytes for the fixture plaintext).
    """
    key = bytes.fromhex(key_hex)
    iv = b'\x00' * KEY_BYTES
    padder = padding.PKCS7(128).padder()
    padded = padder.update(TEST_PLAINTEXT) + padder.finalize()
    encryptor = Cipher(algorithms.AES(key), modes.CBC(iv)).encryptor()
    return (encryptor.update(padded) + encryptor.finalize()).hex()


def format_c_block(hex_str: str, bytes_per_line: int = 8) -> str:
    """Format a hex string as the body of a C array initialiser.

    Returns
    -------
    str
        Multi-line string ready to splice between ``{`` and ``};``.
    """
    byte_count = len(hex_str) // 2
    lines: list[str] = []
    for line_start in range(0, byte_count, bytes_per_line):
        line_end = min(line_start + bytes_per_line, byte_count)
        literals = [f'0x{hex_str[i * 2:i * 2 + 2]}' for i in range(line_start, line_end)]
        is_last = line_end == byte_count
        suffix = '' if is_last else ','
        lines.append(f'    {", ".join(literals)}{suffix}')
    return '\n'.join(lines)


def replace_block(path: Path, identifier: str, length: int, body: str) -> None:
    """Rewrite the body of ``static const unsigned char <identifier>[<length>] = { ... };``.

    Raises
    ------
    RuntimeError
        If the file does not contain exactly one matching block.
    """
    pattern = re.compile(
        rf'(static const unsigned char {re.escape(identifier)}'
        rf'\[{length}\] = \{{\n).*?(\n\}};)',
        re.DOTALL,
    )
    text = path.read_text(encoding='utf-8')
    new_text, count = pattern.subn(lambda m: m.group(1) + body + m.group(2), text)
    if count != 1:
        msg = f'Expected exactly one match for {identifier}[{length}] in {path}; got {count}.'
        raise RuntimeError(msg)
    path.write_text(new_text, encoding='utf-8')


def replace_line(path: Path, pattern: str, replacement: str) -> None:
    """Replace the first whole line matching ``pattern`` with ``replacement``.

    Raises
    ------
    RuntimeError
        If the file does not contain a line matching ``pattern``.
    """
    rx = re.compile(pattern, re.MULTILINE)
    text = path.read_text(encoding='utf-8')
    new_text, count = rx.subn(replacement, text, count=1)
    if count != 1:
        msg = f'No match for {pattern!r} in {path}.'
        raise RuntimeError(msg)
    path.write_text(new_text, encoding='utf-8')


def main() -> int:
    """Rewrite the DBeaver decryption key across every source flavour.

    Returns
    -------
    int
        Process exit code.

    Raises
    ------
    RuntimeError
        If the recomputed ciphertext has an unexpected length.
    """
    parser = argparse.ArgumentParser(
        description='Rotate the DBeaver decryption key across all source flavours.')
    parser.add_argument(
        'key',
        nargs='?',
        help='32-character hex key. If omitted, the current key is fetched from upstream.',
    )
    args = parser.parse_args()

    key_hex = args.key.lower() if args.key else fetch_key()
    if len(key_hex) != KEY_HEX_CHARS:
        parser.error(f'Expected {KEY_HEX_CHARS} hex characters; got {len(key_hex)}.')
    try:
        int(key_hex, 16)
    except ValueError:
        parser.error(f'{key_hex!r} is not a valid hex string.')

    repo = Path(__file__).resolve().parent
    cipher_hex = encrypt_test_plaintext(key_hex)
    if len(cipher_hex) != CIPHER_HEX_CHARS:
        msg = (f'Cipher recomputation produced {len(cipher_hex)} hex chars; '
               f'expected {CIPHER_HEX_CHARS}.')
        raise RuntimeError(msg)

    c_key_block = format_c_block(key_hex)
    c_cipher_block = format_c_block(cipher_hex)

    replace_line(repo / 'dbeaver-creds', r'^KEY=.*$', f'KEY={key_hex}')
    replace_line(repo / 'dbeaver-creds.bat', r'^set KEY=.*$', f'set KEY={key_hex}')
    replace_line(
        repo / 'DBeaverCreds' / 'DBeaverCreds.psm1',
        r"^  \$KEY = '[^']*'$",
        f"  $KEY = '{key_hex}'",
    )

    replace_block(repo / 'src' / 'api.c', 'kKey', 16, c_key_block)
    replace_block(repo / 'tests' / 'test_backend.c', 'k_key', 16, c_key_block)
    replace_block(repo / 'tests' / 'test_backend.c', 'k_cipher', 48, c_cipher_block)
    replace_block(repo / 'tests' / 'test_api.c', 'k_cipher', 48, c_cipher_block)

    return 0


if __name__ == '__main__':
    sys.exit(main())
