@{
Author = 'Tatsh'
CompanyName = ''
CompatiblePSEditions = @('Windows', 'Linux', 'MacOS')
Copyright = '(c) 2023 Tatsh. All rights reserved.'
Description = 'Decrypt and display your DBeaver credentials. The output is a JSON string.'
GUID = '3d7b60e1-9dce-4b70-9017-989227a491a6'
HelpInfoURI = 'https://github.com/Tatsh/dbeaver-creds'
ModuleVersion = '0.0.2'
PowerShellVersion = '5.1'
RequiredAssemblies = @()
RequiredModules = @()
RootModule = 'DBeaverCreds'

AliasesToExport = @('dbeaver-creds')
CmdletsToExport = @()
FunctionsToExport = @('Show-DBeaver-Credential-Json')
VariablesToExport = '*'

PrivateData = @{
    PSData = @{
        IconUri = 'https://avatars.githubusercontent.com/u/724848?s=96&v=4'
        LicenseUri = 'https://github.com/Tatsh/dbeaver-creds/blob/master/LICENSE.txt'
        ProjectUri = 'https://github.com/Tatsh/dbeaver-creds'
        ReleaseNotes = 'https://github.com/Tatsh/dbeaver-creds/blob/master/CHANGELOG.md'
        Tags = @('.net', 'credential', 'database', 'dbeaver', 'password')
    }
}
}
