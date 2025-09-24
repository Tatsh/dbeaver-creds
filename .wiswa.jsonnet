{
  project_type: 'other',
  project_name: 'dbeaver-creds',
  version: '0.0.3',
  description: "Decrypt DBeaver's credentials-config.json file and display the output (a JSON string).",
  keywords: ['authentication', 'dbeaver', 'credentials', 'terminal'],
  want_main: false,
  want_codeql: false,
  want_tests: false,
  copilot+: {
    intro: 'dbeaver-creds is a simple tool to display DBeaver credentials in the terminal.',
  },
  package_json+: {
    scripts: {
      'check-formatting': "prettier -c . && shfmt -d -i 4 -ci -sr dbeaver-creds update-key.sh && markdownlint-cli2 '**/*.md' '#node_modules'",
      'check-spelling': 'cspell --no-progress .',
      format: 'yarn shfmt && yarn prettier -w .',
      qa: 'yarn shellcheck && yarn check-spelling && yarn check-formatting',
      shellcheck: 'shellcheck *.sh dbeaver-creds',
      shfmt: "prettier -w . && shfmt -w -i 4 -ci -sr dbeaver-creds update-key.sh && markdownlint-cli2 --fix '**/*.md' '#node_modules'",
      'update-key': 'bash update-key.sh',
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
  cz+: {
    commitizen+: {
      version_files+: [
        'DBeaverCreds/DBeaverCreds.psd1',
        'man/dbeaver-creds.1',
      ],
    },
  },
}
