<!-- markdownlint-configure-file {"MD024": { "siblings_only": true } } -->

# dbeaver-creds

<!-- WISWA-GENERATED-README:START -->

[![Python versions](https://img.shields.io/pypi/pyversions/dbeaver-creds.svg?color=blue&logo=python&logoColor=white)](https://www.python.org/)
[![PyPI - Version](https://img.shields.io/pypi/v/dbeaver-creds)](https://pypi.org/project/dbeaver-creds/)
[![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/dbeaver-creds)](https://github.com/Tatsh/dbeaver-creds/tags)
[![License](https://img.shields.io/github/license/Tatsh/dbeaver-creds)](https://github.com/Tatsh/dbeaver-creds/blob/master/LICENSE.txt)
[![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/dbeaver-creds/v0.0.3/master)](https://github.com/Tatsh/dbeaver-creds/compare/v0.0.3...master)
[![QA](https://github.com/Tatsh/dbeaver-creds/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/dbeaver-creds/actions/workflows/qa.yml)
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

You can use this in one of three ways depending on your platform and shell:

- A Bash script that works on Linux, macOS and Windows. It requires `openssl` and `dd` to be in `PATH`.
- A [PowerShell module](https://www.powershellgallery.com/packages/DBeaverCreds) that exposes function
  `Show-DBeaver-Credential-Json` (alias `dbeaver-creds`).
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
dbeaver-creds [PATH]
```

If `PATH` is given, the file at that location is decrypted. Otherwise the
platform-default credentials path is used. If the credentials file cannot be
found, the exit status will be > 0.

### PowerShell

```shell
Show-DBeaver-Credential-Json
# or the alias:
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
