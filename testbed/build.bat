@ECHO OFF
REM Build script for testbed executable on Windows

SetLocal EnableDelayedExpansion

REM Recursively collect all .c files
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM Testbed application name
SET assembly=testbed

REM Compiler flags:
REM   -g : Debug symbols
SET compilerFlags=-g 

REM Include paths:
REM   -Isrc              : Local headers
REM   -I../engine/src/   : Headers from engine
SET includeFlags=-Isrc -I../engine/src/

REM Linker flags:
REM   -L../bin/     : Search path for libraries
REM   -lengine.lib  : Link against the engine DLL import library
SET linkerFlags=-L../bin/ -lengine.lib

REM Preprocessor defines:
REM -D_DEBUG : Enable debug mode
REM -DKIMPORT : Mark functions for import from DLL
SET defines=-D_DEBUG -DKIMPORT

REM Output message
ECHO "Building %assembly%..."

REM Run Clang to compile and link the testbed executable
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%