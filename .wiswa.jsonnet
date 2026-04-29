local utils = import 'utils.libsonnet';

{
  uses_user_defaults: true,
  local settings = self,
  description: "Decrypt DBeaver's credentials-config.json file and display the output (a JSON string).",
  keywords: ['authentication', 'credentials', 'dbeaver', 'terminal'],
  project_name: 'dbeaver-creds',
  version: '0.0.3',
  want_codeql: false,
  want_tests: false,
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
        ],
      },
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
          define: { BUILD_CLI: 'OFF', BUILD_PYTHON_EXTENSION: 'ON' },
          version: '>=3.26',
        },
        'minimum-version': 'build-system.requires',
        wheel: {
          exclude: ['**/*.c', '**/*.h', '**/CMakeLists.txt'],
          packages: ['dbeaver_creds'],
        },
      },
    },
  },
  package_json+: {
    scripts: {
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
    '*.ps1',
    '*.psd1',
    '*.psm1',
  ],
  gitignore+: ['/coverage/'],
}
