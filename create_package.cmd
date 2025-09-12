@ECHO OFF
SETLOCAL
CD /D %~dp0

REM -------------------------------------

IF NOT EXIST "_out\ReleaseAMD64\VirtualDub2.exe" (
  ECHO VirtualDub2 not compiled.
  GOTO :END
)

IF NOT EXIST "..\avlib_vdplugin\_bin\Release_x64\avlib-1.vdplugin" (
  ECHO avlib_vdplugin not compiled.
  GOTO :END
)

IF NOT EXIST "..\avlib_vdplugin\ffmpeg\bin_x64\avcodec-62.dll" (
  ECHO FFmpeg DLLs not downloaded.
  GOTO :END
)

IF NOT EXIST "..\scripted_vdplugin\_bin\Release_x64\scripted\scripted.vdplugin" (
  ECHO scripted_vdplugin not compiled.
  GOTO :END
)

REM -------------------------------------

FOR /F "tokens=2*" %%A IN (
  'REG QUERY "HKLM\SOFTWARE\7-Zip" /v "Path" 2^>NUL ^| FIND "REG_SZ" ^|^|
   REG QUERY "HKLM\SOFTWARE\Wow6432Node\7-Zip" /v "Path" 2^>NUL ^| FIND "REG_SZ"') DO SET "SEVENZIP=%%B\7z.exe"

IF NOT EXIST "%SEVENZIP%" (
  ECHO 7Zip not found.
  GOTO :END
)

REM -------------------------------------

SET PCKG_DIR=_out\VirtualDub2_v2.4

MKDIR %PCKG_DIR%
MKDIR %PCKG_DIR%\extra
MKDIR %PCKG_DIR%\plugins64

COPY /Y /V "docs\Changes.txt"                   "%PCKG_DIR%\Changes.txt"
COPY /Y /V "LICENSE.txt"                        "%PCKG_DIR%\LICENSE.txt"
COPY /Y /V "Readme.md"                          "%PCKG_DIR%\Readme.md"
COPY /Y /V "_out\ReleaseAMD64\vdub2.exe"        "%PCKG_DIR%\vdub2.exe"
COPY /Y /V "_out\ReleaseAMD64\VirtualDub2.exe"  "%PCKG_DIR%\VirtualDub2.exe"
COPY /Y /V "_out\ReleaseAMD64\VirtualDub2.vdi"  "%PCKG_DIR%\VirtualDub2.vdi"
COPY /Y /V "dist\VirtualDub.chm"                "%PCKG_DIR%\VirtualDub.chm"

COPY /Y /V "_out\ReleaseAMD64\auxsetup.exe"     "%PCKG_DIR%\extra\auxsetup.exe"
COPY /Y /V "_out\ReleaseAMD64\vdicmdrv.dll"     "%PCKG_DIR%\extra\vdicmdrv.dll"
COPY /Y /V "_out\ReleaseAMD64\vdlaunch.exe"     "%PCKG_DIR%\extra\vdlaunch.exe"
COPY /Y /V "_out\ReleaseAMD64\vdremote.dll"     "%PCKG_DIR%\extra\vdremote.dll"
COPY /Y /V "_out\ReleaseAMD64\vdsvrlnk.dll"     "%PCKG_DIR%\extra\vdsvrlnk.dll"
COPY /Y /V "dist\plugins64\."                   "%PCKG_DIR%\plugins64\."

COPY /Y /V "..\avlib_vdplugin\ffmpeg\bin_x64\avcodec-62.dll"      "%PCKG_DIR%\avcodec-62.dll"
COPY /Y /V "..\avlib_vdplugin\ffmpeg\bin_x64\avformat-62.dll"     "%PCKG_DIR%\avformat-62.dll"
COPY /Y /V "..\avlib_vdplugin\ffmpeg\bin_x64\avutil-60.dll"       "%PCKG_DIR%\avutil-60.dll"
COPY /Y /V "..\avlib_vdplugin\ffmpeg\bin_x64\swresample-6.dll"    "%PCKG_DIR%\swresample-6.dll"
COPY /Y /V "..\avlib_vdplugin\ffmpeg\bin_x64\swscale-9.dll"       "%PCKG_DIR%\swscale-9.dll"

COPY /Y /V "..\avlib_vdplugin\_bin\Release_x64\avlib-1.vdplugin"  "%PCKG_DIR%\plugins64\avlib-1.vdplugin"
COPY /Y /V "..\avlib_vdplugin\Readme.md"                          "%PCKG_DIR%\plugins64\avlib-1_Readme.md"
COPY /Y /V "..\avlib_vdplugin\history.txt"                        "%PCKG_DIR%\plugins64\avlib-1_history.txt"

COPY /Y /V "..\scripted_vdplugin\_bin\Release_x64\scripted\scripted.vdplugin" "%PCKG_DIR%\plugins64\scripted.vdplugin"
COPY /Y /V "..\scripted_vdplugin\Readme.md"                                   "%PCKG_DIR%\plugins64\scripted_Readme.md"
COPY /Y /V "..\scripted_vdplugin\history.txt"                                 "%PCKG_DIR%\plugins64\scripted_history.txt"

REM -------------------------------------

FOR /F "USEBACKQ" %%F IN (`powershell -NoLogo -NoProfile -Command ^(Get-Item "%PCKG_DIR%\VirtualDub2.exe"^).VersionInfo.FileVersion`) DO (SET FILE_VERSION=%%F)

SET PCKG_NAME=VirtualDub2_v%FILE_VERSION%

"%SEVENZIP%" a -m0=lzma -mx9 -ms=on "_out\%PCKG_NAME%.7z" ".\%PCKG_DIR%"

:END
ENDLOCAL
TIMEOUT /T 5
EXIT /B
