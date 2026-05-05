local utils = import 'utils.libsonnet';

{
  uses_user_defaults: true,
  local settings = self,
  description: "Decrypt DBeaver's credentials-config.json file and display the output (a JSON string).",
  keywords: ['authentication', 'credentials', 'dbeaver', 'terminal'],
  project_name: 'dbeaver-creds',
  version: '0.1.0',
  security_policy_supported_versions: { '0.1.x': ':white_check_mark:' },
  want_codeql: false,
  want_tests: false,
  custom_project_badges: [
    {
      anchor: '[![Tests](https://github.com/Tatsh/dbeaver-creds/actions/workflows/tests.yml/badge.svg)]',
      href: 'https://github.com/Tatsh/dbeaver-creds/actions/workflows/tests.yml',
    },
    {
      anchor: '[![Coverage Status](https://coveralls.io/repos/github/Tatsh/dbeaver-creds/badge.svg?branch=master)]',
      href: 'https://coveralls.io/github/Tatsh/dbeaver-creds?branch=master',
    },
    {
      anchor: '[![C](https://img.shields.io/badge/C-00599C?logo=c)]',
      href: '<https://en.wikipedia.org/wiki/C_(programming_language)>',
    },
    {
      anchor: '[![CMake](https://img.shields.io/badge/CMake-6E6E6E?logo=cmake)]',
      href: 'https://cmake.org/',
    },
    {
      anchor: '[![PowerShell Gallery Version (including pre-releases)](https://img.shields.io/powershellgallery/v/DBeaverCreds)]',
      href: 'https://www.powershellgallery.com/packages/DBeaverCreds',
    },
    {
      anchor: '[![PowerShell Gallery Platform Support](https://img.shields.io/powershellgallery/p/DBeaverCreds?label=powershell+platforms+supported)]',
      href: 'https://www.powershellgallery.com/packages/DBeaverCreds',
    },
    {
      anchor: '[![PowerShell Gallery Downloads](https://img.shields.io/powershellgallery/dt/DBeaverCreds)]',
      href: 'https://www.powershellgallery.com/packages/DBeaverCreds',
    },
  ],
  shared_ignore+: [
    '*.so',
    '/compile_commands.json',
    '/.vs/*',
    '!/.vs/launch.vs.json',
  ],
  pyproject+: {
    'build-system': {
      'build-backend': 'scikit_build_core.build',
      requires: ['scikit-build-core>=0.10'],
    },
    project+: {
      classifiers: utils.pyprojectClassifiers(settings, [
        'Environment :: Console',
        'Intended Audience :: Developers',
        'Intended Audience :: System Administrators',
        'Operating System :: MacOS',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: C',
        'Topic :: Database',
        'Topic :: Utilities',
      ]),
      dependencies: ['click>=8.3.2'],
      scripts: { 'dbeaver-creds': 'dbeaver_creds.main:main' },
    },
    tool+: {
      commitizen+: {
        version_files+: [
          'CMakeLists.txt',
          'DBeaverCreds/DBeaverCreds.psd1',
          'man/dbeaver-creds.1',
          'man/dbeaver-creds.h.3',
          'snapcraft.yaml',
        ],
      },
      hatch:: null,
      poetry+: {
        group+: {
          dev+: {
            dependencies+: {
              cryptography: utils.latestPypiPackageVersionCaret('cryptography'),
            },
          },
        },
      },
      'scikit-build': {
        cmake: {
          version: '>=3.26',
          define: {
            BUILD_CLI: 'OFF',
            BUILD_PYTHON_EXTENSION: 'ON',
          },
        },
        'minimum-version': 'build-system.requires',
        sdist: {
          exclude: [
            '**',
          ],
          include: [
            '/CMakeLists.txt',
            '/LICENSE.txt',
            '/README.md',
            '/dbeaver_creds/**/*.c',
            '/dbeaver_creds/**/*.py',
            '/dbeaver_creds/**/*.pyi',
            '/dbeaver_creds/CMakeLists.txt',
            '/dbeaver_creds/py.typed',
            '/include/**/*.h',
            '/man/dbeaver-creds.1',
            '/pyproject.toml',
            '/src/**/*.c',
            '/src/**/*.h',
            '/src/CMakeLists.txt',
          ],
        },
        wheel: {
          exclude: [
            '**/*.c',
            '**/*.h',
            '**/CMakeLists.txt',
          ],
          packages: [
            'dbeaver_creds',
          ],
        },
      },
    },
  },
  package_json+: {
    scripts+: {
      'check-formatting':
        'prettier -c . && shfmt -d -i 4 -ci -sr dbeaver-creds && uv run yapf --diff --parallel --recursive . && markdownlint-cli2 --config package.json --configPointer /markdownlint-cli2',
      'check-spelling': 'cspell --no-progress .',
      'dict:update': "rm -f .vscode/dictionary.txt && cspell lint --no-progress --no-summary --unique --words-only | tr '[:upper:]' '[:lower:]' | sort -u > .vscode/dictionary.txt",
      format:
        'prettier -w . && shfmt -w -i 4 -ci -sr dbeaver-creds && uv run yapf --in-place --parallel --recursive . && markdownlint-cli2 --config package.json --configPointer /markdownlint-cli2 --fix',
      mypy: 'uv run mypy',
      qa: 'yarn shellcheck && yarn mypy . && yarn ruff . && uv run ty check && yarn check-spelling && yarn check-formatting',
      ruff: 'uv run ruff check',
      'ruff:fix': 'uv run ruff check --fix',
      shellcheck: 'shellcheck dbeaver-creds',
      shfmt: 'shfmt -w -i 4 -ci -sr dbeaver-creds',
      test: 'cmake -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug && cmake --build build && ctest --test-dir build --output-on-failure',
      ty: 'uv run ty check',
      'update-key': 'python3 update-key.py',
    },
  },
  vscode+: {
    settings+: {
      '[powershell]': {
        'editor.defaultFormatter': 'ms-vscode.powershell',
      },
    },
  },
  prettierignore+: [
    '*.3',
    '*.bat',
    '*.c',
    '*.h',
    '*.in',
    '*.ps1',
    '*.psd1',
    '*.psm1',
  ],
  gitignore+: ['/coverage/'],
}
