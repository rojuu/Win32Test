@echo off
SETLOCAL
set TARGET=x64
set EXE_NAME=Win32test

set CommonCompilerFlags=/Zi /Od /EHsc /nologo /FC

set CommonLinkerFlags=/DEBUG User32.lib Gdi32.lib

if not exist bin (
    mkdir bin
)

pushd bin
cl %CommonCompilerFlags% ..\src\main.cpp /link /subsystem:windows %CommonLinkerFlags% /out:%EXE_NAME%.exe
popd
echo Done
