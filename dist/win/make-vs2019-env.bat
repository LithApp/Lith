Rem #Source: https://gist.github.com/vvuk/01dc8a12678d1beffaa1e26549d03b02
Rem #Author: Vladimir Vukicevic (vvuk)

@ECHO OFF

set OLDPATH=%PATH%

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64 > NUL:

echo export INCLUDE='%INCLUDE%'
echo export LIB='%LIB%'
echo export LIBPATH='%LIBPATH%'

call set NEWPATH=%%PATH:%OLDPATH%=%%
set NEWPATH=%NEWPATH:C:=/c%
set NEWPATH=%NEWPATH:\=/%
set NEWPATH=%NEWPATH:;=:%
echo export PATH="%NEWPATH%:$PATH"
