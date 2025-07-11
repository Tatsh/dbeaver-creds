$VerbosePreference="Continue"
BeforeAll {
  Import-Module -Force "${PSScriptRoot}/DBeaverCreds.psm1"
}

Describe 'DBeaverCreds' {
  BeforeEach {
    Mock -ModuleName DBeaverCreds CreateAes { }
    Mock -ModuleName DBeaverCreds -RemoveParameterType @('JsonData','AesAlg') DecryptData { return "f" }
    Mock -ModuleName DBeaverCreds -RemoveParameterType @('Bytes') GetString { return "f" }
  }

  It 'Succeeds' {
    Mock -ModuleName DBeaverCreds ReadAllBytes { return 'fffffffffffffffffffffffffffffffff' }
    $data = Show-DBeaver-Credential-Json
    Should -Invoke -ModuleName DBeaverCreds -CommandName ReadAllBytes -Times 1 -Exactly
    Should -Invoke -ModuleName DBeaverCreds -CommandName CreateAes -Times 1 -Exactly
    Should -Invoke -ModuleName DBeaverCreds -CommandName DecryptData -Times 1 -Exactly
    Should -Invoke -ModuleName DBeaverCreds -CommandName GetString -Times 1 -Exactly `
      -ParameterFilter { $Index -eq 16 && $Count -eq 17 }
    $data | Should -Be 'f'
  }

  It "Fails 'gracefully' when ReadAllBytes throws" {
    Mock -ModuleName DBeaverCreds ReadAllBytes { throw }
    $block = { Show-DBeaver-Credential-Json -ErrorAction Stop }
    $block | Should -Throw
    Should -Invoke -ModuleName DBeaverCreds -CommandName ReadAllBytes -Times 1 -Exactly
    Should -Invoke -ModuleName DBeaverCreds -CommandName CreateAes -Times 0 -Exactly
    Should -Invoke -ModuleName DBeaverCreds -CommandName DecryptData -Times 0 -Exactly
    Should -Invoke -ModuleName DBeaverCreds -CommandName GetString -Times 0 -Exactly
  }
}

Describe 'GetJsonPath' {
  It 'Returns a Linux Path' {
    InModuleScope DBeaverCreds {
      $env:XDG_DATA_HOME = $null
      GetJsonPath $true $false | Should -Match '\.local/share/'
    }
  }

  It 'Returns a Linux path using XDG variable' {
    InModuleScope DBeaverCreds {
      $env:XDG_DATA_HOME = '/home/user/.fixed'
      GetJsonPath $true $false | Should -Match '/home/user/\.fixed'
    }
  }

    It 'Returns a macOS path' {
    InModuleScope DBeaverCreds {
      GetJsonPath -Linux $false -Mac $true | Should -Match '/Library/'
    }
  }
}
