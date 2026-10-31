@ECHO OFF

set OLDPATH=%PATH%

for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set VS_PATH=%%i
call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64 > NUL:

echo export INCLUDE='%INCLUDE%'
echo export LIB='%LIB%'
echo export LIBPATH='%LIBPATH%'

call set NEWPATH=%%PATH:%OLDPATH%=%%
set NEWPATH=%NEWPATH:C:=/c%
set NEWPATH=%NEWPATH:\=/%
set NEWPATH=%NEWPATH:;=:%
echo export PATH="%NEWPATH%:$PATH"
