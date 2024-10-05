@ECHO OFF
SETLOCAL
CD /D %~dp0

REM -------------------------------------

FOR /F "tokens=2*" %%A IN (
  'REG QUERY "HKLM\SOFTWARE\7-Zip" /v "Path" 2^>NUL ^| FIND "REG_SZ" ^|^|
   REG QUERY "HKLM\SOFTWARE\Wow6432Node\7-Zip" /v "Path" 2^>NUL ^| FIND "REG_SZ"') DO SET "SEVENZIP=%%B\7z.exe"

IF NOT EXIST "%SEVENZIP%" (
  ECHO 7Zip not found.
  GOTO :END
)

REM -------------------------------------

MKDIR _bin\plugins64

COPY /Y /V "_bin\Release_x64\scripted\scripted.vdplugin" "_bin\plugins64\scripted.vdplugin"
COPY /Y /V "Readme.md" "_bin\plugins64\scripted_Readme.md"
COPY /Y /V "history.txt" "_bin\plugins64\scripted_history.txt"

REM -------------------------------------

FOR /F "USEBACKQ" %%F IN (`powershell -NoLogo -NoProfile -Command ^(Get-Item "_bin\plugins64\scripted.vdplugin"^).VersionInfo.FileVersion`) DO (SET FILE_VERSION=%%F)

SET PCKG_NAME=scripted_vdplugin_%FILE_VERSION%

"%SEVENZIP%" a -m0=lzma -mx9 -ms=on "_bin\%PCKG_NAME%.7z" ^
.\_bin\plugins64

IF EXIST "_bin\plugins64" RD /Q /S "_bin\plugins64"

:END
ENDLOCAL
TIMEOUT /T 5
EXIT /B
