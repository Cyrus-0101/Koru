#!/bin/bash
# Build script for testbed executable on Linux

# Enable echoing of commands as they run
set -x

# Create output directory if it doesn't exist
mkdir -p ../bin

# Find all .c files in current directory and subdirectories
cFilenames=$(find . -type f -name "*.c")

# Name of the resulting executable
assembly="testbed"

# Compiler flags:
#   -g           : Include debug info
#   -fdeclspec   : Support dllexport/dllimport attributes
#   -fPIC        : Position Independent Code (good for linking with shared libs)
compilerFlags="-g -fdeclspec -fPIC" 

# -fms-extensions 
# -Wall -Werror

# Include paths:
#   -Isrc              : Local headers
#   -I../engine/src/   : Headers from engine
includeFlags="-Isrc -I../engine/src/"

# Linker flags:
#   -L../bin/     : Look for libraries in ../bin/
#   -lengine      : Link against libengine.so
#   -Wl,-rpath,.  : Set runtime path to current dir so it finds the .so
linkerFlags="-L../bin/ -lengine -Wl,-rpath,."

# Preprocessor defines:
#   -D_DEBUG    : Enable debug mode
#   -DKIMPORT   : Mark functions for import from shared library
defines="-D_DEBUG -DKIMPORT"

# Output message
echo "Building $assembly..."

# Echo full command line before running (for debugging)
echo clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags

# Run clang to build the final executable
clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags