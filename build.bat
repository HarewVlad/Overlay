@echo off

REM ctime -begin handmade_hero.ctm

REM set CommonCompilerFlags=-diagnostics:column -WL -O2 -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999
REM set CommonCompilerFlags=-DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 %CommonCompilerFlags%
REM set CommonLinkerFlags=-STACK:0x100000,0x100000 -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib kernel32.lib

REM x86 or x64
set Machine=%1

set LinkerFlags=-incremental:no user32.lib gdi32.lib winmm.lib kernel32.lib
set InjectorLinkerFlags=%LinkerFlags% 
set OverlayLinkerFlags=-dll %LinkerFlags% vendor/libs/%Machine%/detours/detours.lib

REM Build injector
cl -Zi -nologo injector/main.cpp /link %InjectorLinkerFlags% -out:injector_%Machine%.exe

REM Build overlay
cl -D_WINDLL -D_USRDLL -nologo overlay/main.cpp /link %OverlayLinkerFlags% -out:overlay_%Machine%.dll

REM Build overlay to test
cl -Zi -nologo overlay/main.cpp /link %LinkerFlags% vendor/libs/%Machine%/detours/detours.lib -out:overlay_%Machine%_gui.exe

REM ctime -end handmade_hero.ctm %LastError%
