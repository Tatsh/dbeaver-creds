<!-- markdownlint-configure-file {"MD024": { "siblings_only": true } } -->

# dbeaver-creds

[![PowerShell Gallery Version (including pre-releases)](https://img.shields.io/powershellgallery/v/DBeaverCreds)](https://www.powershellgallery.com/packages/DBeaverCreds)
[![PowerShell Gallery Platform Support](https://img.shields.io/powershellgallery/p/DBeaverCreds?label=powershell+platforms+supported)](https://www.powershellgallery.com/packages/DBeaverCreds)
[![PowerShell Gallery Downloads](https://img.shields.io/powershellgallery/dt/DBeaverCreds)](https://www.powershellgallery.com/packages/DBeaverCreds)
[![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/dbeaver-creds)](https://github.com/Tatsh/dbeaver-creds/tags)
[![License](https://img.shields.io/github/license/Tatsh/dbeaver-creds)](https://github.com/Tatsh/dbeaver-creds/blob/master/LICENSE.txt)
[![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/dbeaver-creds/v0.0.3/master)](https://github.com/Tatsh/dbeaver-creds/compare/v0.0.3...master)
[![QA](https://github.com/Tatsh/dbeaver-creds/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/dbeaver-creds/actions/workflows/qa.yml)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)
[![Tests](https://github.com/Tatsh/dbeaver-creds/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/dbeaver-creds/actions/workflows/tests.yml)
[![Coverage Status](https://coveralls.io/repos/github/Tatsh/dbeaver-creds/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/dbeaver-creds?branch=master)
[![GitHub Pages](https://github.com/Tatsh/dbeaver-creds/actions/workflows/pages/pages-build-deployment/badge.svg)](https://tatsh.github.io/dbeaver-creds/)
[![Stargazers](https://img.shields.io/github/stars/Tatsh/dbeaver-creds?logo=github&style=flat)](https://github.com/Tatsh/dbeaver-creds/stargazers)

[![@Tatsh](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fpublic.api.bsky.app%2Fxrpc%2Fapp.bsky.actor.getProfile%2F%3Factor%3Ddid%3Aplc%3Auq42idtvuccnmtl57nsucz72%26query%3D%24.followersCount%26style%3Dsocial%26logo%3Dbluesky%26label%3DFollow%2520%40Tatsh&query=%24.followersCount&style=social&logo=bluesky&label=Follow%20%40Tatsh)](https://bsky.app/profile/Tatsh.bsky.social)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109370961877277568?domain=hostux.social&style=social)](https://hostux.social/@Tatsh)

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
