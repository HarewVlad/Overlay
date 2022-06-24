@echo off

REM ctime -begin handmade_hero.ctm

REM set CommonCompilerFlags=-diagnostics:column -WL -O2 -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999
REM set CommonCompilerFlags=-DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 %CommonCompilerFlags%
REM set CommonLinkerFlags=-STACK:0x100000,0x100000 -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib kernel32.lib

REM x86 or x64
set Machine=%1

set InjectorCompilerFlags=-Zi
set OverlayCompilerFlags=-D_WINDLL -D_USRDLL

set LinkerFlags=-incremental:no user32.lib gdi32.lib winmm.lib kernel32.lib vendor/libs/%Machine%/detours/detours.lib
set InjectorLinkerFlags=%LinkerFlags%
set OverlayLinkerFlags=-dll %LinkerFlags%

REM Build Injector
cl %InjectorCompilerFlags% main.cpp /link %InjectorLinkerFlags% -out:injector_%Machine%.exe

REM Build Overlay
cl %OverlayCompilerFlags% main.cpp /link %OverlayLinkerFlags% -out:overlay_%Machine%.dll

REM 32-bit build
REM cl %CommonCompilerFlags% ..\handmade\code\win32_handmade.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
REM Optimization switches /wO2
REM echo WAITING FOR PDB > lock.tmp

REM ctime -end handmade_hero.ctm %LastError%
