@echo off

REM x86 or x64
set Machine=%1

set CompilerFlags=/Zi /nologo /GR- /EHs- /fp:fast
set LinkerFlags=/incremental:no user32.lib gdi32.lib winmm.lib kernel32.lib
set OveralyLibs=vendor/libs/%Machine%/detours/detours.lib Ole32.lib
set OverlayLinkerFlags=%LinkerFlags% %OveralyLibs% /out:overlay_%Machine%_gui.exe

REM Build injector
cl %CompilerFlags% injector/main.cpp /link %LinkerFlags% /out:injector_%Machine%.exe

REM Build overlay
cl %CompilerFlags% -D_WINDLL -D_USRDLL /MD /I vendor\include\ overlay/main.cpp /link -dll %OverlayLinkerFlags%

REM Build overlay to test
cl %CompilerFlags% /I vendor\include\ overlay/main.cpp /link %OverlayLinkerFlags%