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
