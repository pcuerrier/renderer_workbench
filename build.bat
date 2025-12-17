@echo off

set INCLUDES=-I..\src\
set CommonCompilerFlags=-diagnostics:column -WL -Od -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -Wall -FC -Z7 -GS- -Gs9999999
set CommonCompilerFlags=-DDEBUG %CommonCompilerFlags% %INCLUDES%
set CommonLinkerFlags=-STACK:0x100000,0x100000 -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib kernel32.lib

IF NOT EXIST .\build mkdir .\build
pushd .\build

del *.pdb > NUL 2> NUL

REM Platforms
cl %CommonCompilerFlags% ..\src\win32\win32_main.cpp -Fmwin32.map /link /SUBSYSTEM:windows %CommonLinkerFlags%

popd
