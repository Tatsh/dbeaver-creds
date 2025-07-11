function CreateAes {
  Param ($Key)
  $aesAlg = [System.Security.Cryptography.Aes]::Create()
  $aesAlg.Key = [byte[]] -split ($Key -replace '..', '0x$& ')
  $aesAlg.IV = [byte[]]::new(16)
  $aesAlg
}

function ReadAllBytes {
  Param ($Path)
  [System.IO.File]::ReadAllBytes(${Path})
}

function GetString {
  Param ([byte[]]$Bytes, [int]$Index, [int]$Count)
  [System.Text.Encoding]::UTF8.GetString($Bytes, $Index, $Count)
}

function DecryptData {
  Param ([byte[]]$JsonData, [System.Security.Cryptography.Aes]$AesAlg)
  $AesAlg.CreateDecryptor().TransformFinalBlock($JsonData, 0, $JsonData.Length)
}

function GetJsonPath {
  Param ([bool]$Linux, [bool]$Mac)
  $LINUX_MAC_SUFFIX = 'DBeaverData/workspace6/General/.dbeaver/credentials-config.json'
  if ($Linux) {
    if ($env:XDG_DATA_HOME) {
      return "${env:XDG_DATA_HOME}/${LINUX_MAC_SUFFIX}"
    }
    return "${env:HOME}/.local/share/${LINUX_MAC_SUFFIX}"
  }
  if ($Mac) {
    return "${env:HOME}/Library/${LINUX_MAC_SUFFIX}"
  }
  "${env:APPDATA}\DBeaverData\workspace6\General\.dbeaver\credentials-config.json"
}

<#
  .SYNOPSIS
  Decrypt and display your DBeaver credentials. The output is a JSON string.

  .EXAMPLE
    # Show the credentials.
    Show-DBeaver-Credential-Json

    # Use the alias.
    dbeaver-creds
#>
function Show-DBeaver-Credential-Json {
  try {
    $configJsonData = ReadAllBytes -Path (GetJsonPath -Linux $IsLinux -Mac $IsMacOs)
  } catch {
    Write-Error -Category ReadError -Message 'credentials-config.json not found or could not be read.'
    return
  }
  $KEY = 'babb4a9f774ab853c96c2d653dfe544a'
  $aesAlg = CreateAes -Key $KEY
  $data = DecryptData -JsonData $configJsonData -AesAlg $aesAlg
  GetString -Bytes $data -Index 16 -Count ($data.Length - 16)
}
Set-Alias -Name dbeaver-creds -Value Show-DBeaver-Credential-Json
Export-ModuleMember -Function Show-DBeaver-Credential-Json -Alias dbeaver-creds
