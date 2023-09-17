@echo off
set KEY=babb4a9f774ab853c96c2d653dfe544a
set CONFIG_JSON=%APPDATA%\DBeaverData\workspace6\General\.dbeaver\credentials-config.json
openssl aes-128-cbc -d -K "%KEY%" -iv 00000000000000000000000000000000 -in "%CONFIG_JSON%" | dd bs=1 skip=16 2> nul
