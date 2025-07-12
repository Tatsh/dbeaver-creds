$VerbosePreference="Continue"
BeforeAll {
  Import-Module -Force "${PSScriptRoot}/DBeaverCreds.psm1"
}

Describe 'DBeaverCreds' {
  BeforeEach {
    Mock -ModuleName DBeaverCreds CreateAes { }
    Mock -ModuleName DBeaverCreds -RemoveParameterType @('JsonData','AesAlg') DecryptData {
      return "f"
    }
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

  Describe 'CreateAes' {
    It 'Creates an Aes object with the correct key' {
      InModuleScope DBeaverCreds {
        $key = 'babb4a9f774ab853c96c2d653dfe544a'
        $aes = CreateAes -Key $key
        $aes.Key | Should -BeOfType Byte
        $aes.Key | Should -HaveCount 16
        $aes.IV | Should -BeOfType Byte
        $aes.IV | Should -HaveCount 16
      }
    }
  }

  Describe 'ReadAllBytes' {
    It 'Reads all bytes from a file' {
      InModuleScope DBeaverCreds {
        $path = 'test.json'
        Write-Output '{"test": "data"}' | Out-File -FilePath $path
        $data = ReadAllBytes -Path $path
        $data | Should -BeOfType Byte
        $data | Should -HaveCount 17
        Remove-Item -Path $path -Force
      }
    }
  }

  Describe 'GetString' {
    It 'Converts byte array to string' {
      InModuleScope DBeaverCreds {
        $bytes = [byte[]]@(102, 111, 111, 98, 97, 114)
        $result = GetString -Bytes $bytes -Index 0 -Count $bytes.Length
        $result | Should -Be 'foobar'
      }
    }
  }

  Describe 'DecryptData' {
    It 'Decrypts data using AES algorithm' {
      InModuleScope DBeaverCreds {
        $aes = CreateAes -Key 'babb4a9f774ab853c96c2d653dfe544a'
        $encryptedData = $aes.CreateEncryptor().TransformFinalBlock(
          [byte[]]@(102, 111, 111, 98, 97, 114), 0, 6)
        $decrypted = DecryptData -JsonData $encryptedData -AesAlg $aes
        $decrypted | Should -BeOfType Byte
        $decrypted | Should -HaveCount 6
      }
    }
  }
}
