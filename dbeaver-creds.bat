@echo off
rem Decrypt and display your DBeaver credentials. Usage: dbeaver-creds.bat [PATH]
set KEY=babb4a9f774ab853c96c2d653dfe544a
if not "%~1" == "" (
  set "CONFIG_JSON=%~1"
) else (
  set CONFIG_JSON=%APPDATA%\DBeaverData\workspace6\General\.dbeaver\credentials-config.json
)
if not exist "%CONFIG_JSON%" (
  echo "credentials-config.json not found or could not be read."
) else (
  openssl aes-128-cbc -d -K "%KEY%" -iv 00000000000000000000000000000000 -in "%CONFIG_JSON%" | dd bs=1 skip=16 2> nul
)
