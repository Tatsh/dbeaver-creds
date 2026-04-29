<!-- markdownlint-configure-file {"MD024": { "siblings_only": true } } -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.1/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Added changelog
- C library and `dbeaver-creds` CLI binary that decrypt DBeaver's `credentials-config.json` without
  requiring `openssl` or `dd`, with selectable cryptography backends:
  - `native` backend using CommonCrypto on macOS and BCrypt on Windows.
  - `openssl` backend for cross-platform builds.
  - Backend selection via the CMake option `DBEAVER_CREDS_BACKEND=native|openssl`, with
    auto-detection by default.
- Optional `[PATH]` argument to the `dbeaver-creds` command to decrypt a credentials file at a
  specific location. When omitted, the command falls back to platform-default discovery.
  - Supported in the C `dbeaver-creds` binary.
  - Supported in the Bash `dbeaver-creds` script.
  - Supported in the Batch `dbeaver-creds.bat` script.
  - Supported in the Python console script `dbeaver-creds`.
- Optional `-Path` parameter to the PowerShell `Show-DBeaver-Credential-Json` function (alias
  `dbeaver-creds`); falls back to platform-default discovery when omitted.
- Public C API function `get_dbeaver_credentials` (declared in `dbeaver-creds.h`) that accepts a
  path argument and returns the decrypted JSON.
- Section 3 manpage `dbeaver-creds.h` documenting the C API.
- Python package `dbeaver-creds` (importable as `dbeaver_creds`) published to PyPI, providing a
  CPython extension that wraps the C library.
  - Public Python API `dbeaver_creds.get_dbeaver_credentials` accepting an optional path
    (`str`, `os.PathLike`, or `None`) and returning the decrypted JSON as a `str`.
  - Console script `dbeaver-creds` installed via pip, accepting an optional `[PATH]` argument.
  - Type stubs and a `py.typed` marker for static type checkers.

### Changed

- Updated the `dbeaver-creds(1)` manpage to document the optional `[PATH]` argument.
- Updated the readme usage section to document the optional path argument across the Bash, Batch,
  and PowerShell script flavours.

### Fixed

- Fixed typo in readme

## [0.0.3] - 2023-09-17

### Changed

- PowerShell module: fix tags for display of platforms supported in badge
- Format PowerShell code

## [0.0.2] - 2023-09-17

### Changed

- Upgrade dependencies

[unreleased]: https://github.com/Tatsh/dbeaver-creds/compare/v0.0.3...HEAD
[0.0.3]: https://github.com/Tatsh/dbeaver-creds/compare/v0.0.2...v0.0.3
[0.0.2]: https://github.com/Tatsh/dbeaver-creds/compare/v0.0.1...v0.0.2
