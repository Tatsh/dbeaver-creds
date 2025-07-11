. "${PSScriptRoot}/utils.ps1"

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
