# dbeaver-creds

Decrypt DBeaver's `credentials-config.json` file. The only dependencies are `openssl` and any
implementation of `dd`.

## Notes

The decryption key is from [the DBeaver source](https://github.com/dbeaver/dbeaver/blob/d69a75e63bf0a00e37f6b4ab9c9aa4fcaa0ded23/plugins/org.jkiss.dbeaver.model/src/org/jkiss/dbeaver/model/impl/app/DefaultSecureStorage.java#L32).

It can be converted to hexadecimal like so in Python:

```python
import struct
as_hex = struct.pack('<16b', 0, 1, 2, etc).hex()
```

`update-key.sh` demonstrates conversion using shell script.

## Development

Requirements:

- Bash
- GNU sed
- Perl
- curl
- cut
- grep
- tr
- Yarn

## Tasks

- `yarn format`: to format the project's files.
- `yarn qa`: Perform a QA check.
- `yarn update-key` or `update-key.sh`: update the key in `dbeaver-creds`.
