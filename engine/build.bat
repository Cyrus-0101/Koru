@ECHO OFF
REM Build script for engine DLL on Windows

SetLocal EnableDelayedExpansion

REM Collect all .c files recursively into cFilenames variable
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM Assembly name (DLL)
SET assembly=engine

REM Compiler flags:
REM   -g               : Debug symbols
REM   -shared          : Build a DLL
REM   -Wvarargs       : Warn on incorrect vararg usage
REM   -Wall/-Werror    : All warnings as errors
SET compilerFlags=-g -shared -Wvarargs -Wall -Werror

REM Include paths:
REM   -Isrc                  : Engine source headers
REM   -I%VULKAN_SDK%\Include  : Vulkan SDK headers
SET includeFlags=-Isrc -I%VULKAN_SDK%\Include

REM Linker flags:
REM   -luser32        : Windows user interface library
REM   -lvulkan-1      : Vulkan dynamic import library
REM   -L...           : Library search path
SET linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%\Lib

REM Preprocessor defines:
REM   -D_DEBUG                   : Enable debug mode
REM   -DKEXPORT                  : Mark functions for export
REM   -D_CRT_SECURE_NO_WARNINGS : Disable CRT deprecation warnings
SET defines=-D_DEBUG -DKEXPORT -D_CRT_SECURE_NO_WARNINGS

REM Output message
ECHO "Building %assembly%..."

REM Run Clang to compile and link the DLL
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.dll %defines% %includeFlags% %linkerFlags%