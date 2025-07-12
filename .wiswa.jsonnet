(import 'defaults.libjsonnet') + {
  // General settings
  project_type: 'other',

  // Shared
  github_username: 'Tatsh',
  security_policy_supported_versions: { '0.0.x': ':white_check_mark:' },
  authors: [
    {
      'family-names': 'Udvare',
      'given-names': 'Andrew',
      email: 'audvare@gmail.com',
      name: '%s %s' % [self['given-names'], self['family-names']],
    },
  ],
  project_name: 'dbeaver-creds',
  version: '0.0.3',
  description: "Decrypt DBeaver's credentials-config.json file and display the output (a JSON string).",
  keywords: ['authentication', 'dbeaver', 'credentials', 'terminal'],
  want_main: false,
  copilot: {
    intro: 'dbeaver-creds is a simple tool to display DBeaver credentials in the terminal.',
  },
  social+: {
    mastodon+: { id: '109370961877277568' },
  },

  // GitHub
  github+: {
    funding+: {
      ko_fi: 'tatsh2',
      liberapay: 'tatsh2',
      patreon: 'tatsh2',
    },
  },
}
