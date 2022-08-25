@echo off

REM x64 build only, who needs x86
REM Make static build for ffmpeg libs

set CompilerFlags=/Zi /nologo /GR- /EHs- /fp:fast
set LinkerFlags=/incremental:no user32.lib gdi32.lib winmm.lib kernel32.lib
set OveralyLibs=vendor/libs/detours/detours.lib vendor/libs/ffmpeg/avcodec.lib vendor/libs/ffmpeg/avdevice.lib vendor/libs/ffmpeg/avfilter.lib vendor/libs/ffmpeg/avformat.lib vendor/libs/ffmpeg/avutil.lib vendor/libs/ffmpeg/swscale.lib Ole32.lib

REM Build injector
cl %CompilerFlags% injector/main.cpp /link %LinkerFlags% /out:injector.exe

REM Build overlay
cl %CompilerFlags% -D_WINDLL -D_USRDLL /MD /I vendor\include\ /I vendor\include\ffmpeg overlay/main.cpp /link -dll %LinkerFlags% %OveralyLibs% /out:overlay.dll

REM Build overlay to test
cl %CompilerFlags% /I vendor\include\ /I vendor\include\ffmpeg overlay/main.cpp /link %LinkerFlags% %OveralyLibs% /out:overlay.exe