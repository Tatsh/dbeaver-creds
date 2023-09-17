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
  $KEY = 'babb4a9f774ab853c96c2d653dfe544a'
  $aesAlg = [System.Security.Cryptography.Aes]::Create()
  $aesAlg.Key = [byte[]] -split ($KEY -replace '..', '0x$& ')
  $aesAlg.IV = [byte[]]::new(16)
  $configJsonData = if ($IsLinux) {
    [System.IO.File]::ReadAllBytes("${env:HOME}/.local/share/DBeaverData/workspace6/General/.dbeaver/credentials-config.json")
  } elseif ($IsMacOS) {
    [System.IO.File]::ReadAllBytes("${env:HOME}/Library/DBeaverData/workspace6/General/.dbeaver/credentials-config.json")
  } else {
    [System.IO.File]::ReadAllBytes("${env:APPDATA}\DBeaverData\workspace6\General\.dbeaver\credentials-config.json")
  }
  $data = $aesAlg.CreateDecryptor().TransformFinalBlock($configJsonData, 0, $configJsonData.Length)
  [System.Text.Encoding]::UTF8.GetString($data, 16, $data.Length - 16)
}
Set-Alias -Name dbeaver-creds -Value Show-DBeaver-Credential-Json
Export-ModuleMember -Function Show-DBeaver-Credential-Json -Alias dbeaver-creds
