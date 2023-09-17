# dbeaver-creds

[![QA](https://github.com/Tatsh/dbeaver-creds/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/dbeaver-creds/actions/workflows/qa.yml)
[![PowerShell Gallery Version (including pre-releases)](https://img.shields.io/powershellgallery/v/DBeaverCreds)](https://www.powershellgallery.com/packages/DBeaverCreds)
[![PowerShell Gallery Platform Support](https://img.shields.io/powershellgallery/p/DBeaverCreds?label=powershell+platforms+supported)](https://www.powershellgallery.com/packages/DBeaverCreds)

Decrypt DBeaver's `credentials-config.json` file and display the output (a JSON string).

You can use this in one of three ways depending on your platform and shell:

- A Bash script that works on Linux, macOS and Windows. It requires `openssl` and `dd` to be in `PATH`.
- A [PowerShell module](https://www.powershellgallery.com/packages/DBeaverCreds) that exposes function
  `Show-DBBeaver-Credential-Json` (alias `dbeaver-creds`).
  This does _not_ require `openssl` or `dd` to function.
- Windows only: A [Batch script](https://en.wikipedia.org/wiki/Batch_file) script equivalent to the
  Bash script and has the same requirements.

## Installation

Place `dbeaver-creds` (or `dbeaver-creds.bat`) in `PATH`.

### PowerShell

```powershell
Install-Module -Name DBeaverCreds
```

## Usage

```shell
dbeaver-creds
```

If the credentials file cannot be found, the exit status will be > 0.

### PowerShell

```shell
Show-DBeaver-Credential-Json
# or the alias:
dbeaver-creds
```

## Development

Requirements:

- Bash
- GNU sed
- Perl
- Yarn
- curl
- cut
- grep
- tr

### Notes

The decryption key is from [the DBeaver source](https://github.com/dbeaver/dbeaver/blob/d69a75e63bf0a00e37f6b4ab9c9aa4fcaa0ded23/plugins/org.jkiss.dbeaver.model/src/org/jkiss/dbeaver/model/impl/app/DefaultSecureStorage.java#L32).

It can be converted to hexadecimal like so in Python:

```python
import struct
as_hex = struct.pack('<16b', 0, 1, 2, etc).hex()
```

`update-key.sh` demonstrates conversion using shell script.

### Tasks

- `yarn format`: to format the project's files.
- `yarn qa`: Perform a QA check.
- `yarn update-key` or `update-key.sh`: update the key in `dbeaver-creds`.
