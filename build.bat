@echo off

REM x86 or x64
set Machine=%1

set CompilerFlags=-Zi -nologo -GR- -EHs- -fp:fast
set LinkerFlags=-incremental:no user32.lib gdi32.lib winmm.lib kernel32.lib
set InjectorLinkerFlags=%LinkerFlags% 
set OverlayLinkerFlags=-dll %LinkerFlags% vendor/libs/%Machine%/detours/detours.lib Ole32.lib

REM Build injector
cl %CompilerFlags% injector/main.cpp /link %InjectorLinkerFlags% -out:injector_%Machine%.exe

REM Build overlay
cl -D_WINDLL -D_USRDLL %CompilerFlags% overlay/main.cpp /link %OverlayLinkerFlags% -out:overlay_%Machine%.dll

REM Build overlay to test
cl %CompilerFlags% overlay/main.cpp /link %LinkerFlags% vendor/libs/%Machine%/detours/detours.lib Ole32.lib -out:overlay_%Machine%_gui.exe