@echo off
SETLOCAL
set TARGET=x64
set EXE_NAME=Win32test

set GLEW=%CD%\libs\glew-2.1.0
set GLEW_BIN=%GLEW%\bin\Release\%TARGET%
set GLEW_INC=%GLEW%\include
set GLEW_LIB=%GLEW%\lib\Release\%TARGET%

set CommonCompilerFlags=/Zi /Od /EHsc /nologo /FC -I%GLEW_INC%

set CommonLinkerFlags=/DEBUG user32.lib -libpath:%GLEW_LIB% gdi32.lib glew32.lib opengl32.lib

if not exist bin (
    mkdir bin
)

pushd bin

if not exist glew32.dll (
    robocopy %GLEW_BIN% . glew32.dll
)

cl %CommonCompilerFlags% ..\src\win32_main.cpp /link /subsystem:windows %CommonLinkerFlags% /out:%EXE_NAME%.exe
popd
echo Done
