<!-- markdownlint-configure-file {"MD024": { "siblings_only": true } } -->

# dbeaver-creds

<!-- WISWA-GENERATED-README:START -->

[![Python versions](https://img.shields.io/pypi/pyversions/dbeaver-creds.svg?color=blue&logo=python&logoColor=white)](https://www.python.org/)
[![PyPI - Version](https://img.shields.io/pypi/v/dbeaver-creds)](https://pypi.org/project/dbeaver-creds/)
[![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/dbeaver-creds)](https://github.com/Tatsh/dbeaver-creds/tags)
[![License](https://img.shields.io/github/license/Tatsh/dbeaver-creds)](https://github.com/Tatsh/dbeaver-creds/blob/master/LICENSE.txt)
[![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/dbeaver-creds/v0.1.0/master)](https://github.com/Tatsh/dbeaver-creds/compare/v0.1.0...master)
[![QA](https://github.com/Tatsh/dbeaver-creds/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/dbeaver-creds/actions/workflows/qa.yml)
[![Tests](https://github.com/Tatsh/dbeaver-creds/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/dbeaver-creds/actions/workflows/tests.yml)
[![Coverage Status](https://coveralls.io/repos/github/Tatsh/dbeaver-creds/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/dbeaver-creds?branch=master)
[![C](https://img.shields.io/badge/C-00599C?logo=c)](<https://en.wikipedia.org/wiki/C_(programming_language)>)
[![CMake](https://img.shields.io/badge/CMake-6E6E6E?logo=cmake)](https://cmake.org/)
[![PowerShell Gallery Version (including pre-releases)](https://img.shields.io/powershellgallery/v/DBeaverCreds)](https://www.powershellgallery.com/packages/DBeaverCreds)
[![PowerShell Gallery Platform Support](https://img.shields.io/powershellgallery/p/DBeaverCreds?label=powershell+platforms+supported)](https://www.powershellgallery.com/packages/DBeaverCreds)
[![PowerShell Gallery Downloads](https://img.shields.io/powershellgallery/dt/DBeaverCreds)](https://www.powershellgallery.com/packages/DBeaverCreds)
[![Dependabot](https://img.shields.io/badge/Dependabot-enabled-blue?logo=dependabot)](https://github.com/dependabot)
[![Documentation Status](https://readthedocs.org/projects/dbeaver-creds/badge/?version=latest)](https://dbeaver-creds.readthedocs.org/?badge=latest)
[![mypy](https://www.mypy-lang.org/static/mypy_badge.svg)](https://mypy-lang.org/)
[![uv](https://img.shields.io/badge/uv-261230?logo=astral)](https://docs.astral.sh/uv/)
[![Ruff](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/astral-sh/ruff/main/assets/badge/v2.json)](https://github.com/astral-sh/ruff)
[![Downloads](https://static.pepy.tech/badge/dbeaver-creds/month)](https://pepy.tech/project/dbeaver-creds)
[![Stargazers](https://img.shields.io/github/stars/Tatsh/dbeaver-creds?logo=github&style=flat)](https://github.com/Tatsh/dbeaver-creds/stargazers)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
[![Prettier](https://img.shields.io/badge/Prettier-black?logo=prettier)](https://prettier.io/)

[![@Tatsh](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fpublic.api.bsky.app%2Fxrpc%2Fapp.bsky.actor.getProfile%2F%3Factor=did%3Aplc%3Auq42idtvuccnmtl57nsucz72&query=%24.followersCount&label=Follow+%40Tatsh&logo=bluesky&style=social)](https://bsky.app/profile/Tatsh.bsky.social)
[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-Tatsh-black?logo=buymeacoffee)](https://buymeacoffee.com/Tatsh)
[![Libera.Chat](https://img.shields.io/badge/Libera.Chat-Tatsh-black?logo=liberadotchat)](irc://irc.libera.chat/Tatsh)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109370961877277568?domain=hostux.social&style=social)](https://hostux.social/@Tatsh)
[![Patreon](https://img.shields.io/badge/Patreon-Tatsh2-F96854?logo=patreon)](https://www.patreon.com/Tatsh2)

<!-- WISWA-GENERATED-README:STOP -->

Decrypt DBeaver's `credentials-config.json` file and display the output (a JSON string).

This project ships in six flavours so you can use it in whichever language or
shell suits you:

- **Python module** (`pip install dbeaver-creds`) - importable as `dbeaver_creds`
  and exposes a `dbeaver-creds` console script. C-extension under the hood, no
  external dependencies on your machine.
- **Native CLI binary** built from this source tree via CMake. Single static
  binary, no runtime dependencies beyond the chosen crypto backend (CommonCrypto
  on macOS, BCrypt on Windows, OpenSSL on Linux).
- **C library** (`#include <dbeaver-creds.h>`) - link
  `dbeaver_creds_core.a` and call `get_dbeaver_credentials(path)`.
- **Bash script** that works on Linux, macOS, and Windows. Requires `openssl`
  and `dd` to be in `PATH`.
- **PowerShell module**
  ([DBeaverCreds](https://www.powershellgallery.com/packages/DBeaverCreds))
  exposing `Show-DBeaver-Credential-Json` (alias `dbeaver-creds`). Pure
  .NET; no external dependencies.
- **Batch script** (Windows only) equivalent to the Bash script with the
  same `openssl` + `dd` requirements.

## Installation

### Python

```shell
pip install dbeaver-creds
```

### Native CLI / C library

```shell
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

`DBEAVER_CREDS_BACKEND=native|openssl` (passed via `-D`) overrides the auto-detected backend
(`native` = CommonCrypto on macOS / BCrypt on Windows; `openssl` =
cross-platform).

### Shell scripts

Place `dbeaver-creds` (or `dbeaver-creds.bat`) in `PATH`.

### PowerShell

```powershell
Install-Module -Name DBeaverCreds
```

## Usage

All entry points accept an optional path argument; if absent, the
platform-default credentials path is used. If the credentials file cannot be
read, the exit status is non-zero.

### Native CLI / Bash / Batch

```shell
dbeaver-creds [PATH]
```

### Python

```python
from dbeaver_creds import get_dbeaver_credentials

print(get_dbeaver_credentials())                  # default path
print(get_dbeaver_credentials('/some/where.json'))  # explicit
```

Or use the console script installed by `pip`:

```shell
dbeaver-creds [PATH]
python -m dbeaver_creds [PATH]
```

### C library

```c
#include <dbeaver-creds.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  enum dbeaver_credentials_error err = DBEAVER_CREDENTIALS_OK;
  char *json = get_dbeaver_credentials(NULL, &err);  // pass a literal path or NULL
  if (!json) {
    fprintf(stderr, "dbeaver-creds failed: %d\n", err);
    return 1;
  }
  puts(json);
  free(json);
  return 0;
}
```

### PowerShell

```powershell
Show-DBeaver-Credential-Json
# Or the alias:
dbeaver-creds
# With an explicit path:
Show-DBeaver-Credential-Json -Path 'C:\custom\credentials-config.json'
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
