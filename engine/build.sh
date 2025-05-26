#!/bin/bash
# Build script for engine shared library (.so) on Linux

# Enable echoing commands as they run (debugging help)
set echo on

# Create the output directory if it doesn't exist
mkdir -p ../bin

# Find all .c files in current directory and subdirectories
cFilenames=$(find . -type f -name "*.c")

# echo "Files:" $cFilenames

# Name of the resulting shared library
assembly="engine"

# Compiler flags:
#   -g            : Include debug info
#   -shared       : Build a shared library (.so)
#   -fdeclspec    : Allow dllexport/dllimport attributes
#   -fPIC         : Position Independent Code (required for shared libraries)
compilerFlags="-g -shared -fdeclspec -fPIC"

# -fms-extensions 
# -Wall -Werror

# Include paths:
#   -Isrc                : Include headers from src/
#   -I$VULKAN_SDK/include: Include Vulkan SDK headers
includeFlags="-Isrc -I$VULKAN_SDK/include"

# Linker flags:
#   -lvulkan             : Link against Vulkan
#   -lxcb                : XCB library for windowing
#   -lX11                : X11 base library
#   -lX11-xcb            : Interop between X11 and XCB
#   -lxkbcommon          : XKB keyboard handling
#   -L...                : Specify search path for libraries
linkerFlags="-lvulkan -lxcb -lX11 -lX11-xcb -lxkbcommon -L$VULKAN_SDK/lib -L/usr/X11R6/lib"

# Preprocessor defines:
#   -D_DEBUG     : Enables debug logging/asserts
#   -DKEXPORT    : Marks functions for export in shared lib
defines="-D_DEBUG -DKEXPORT"

# Output message
echo "Building $assembly..."

# Run clang with all flags to produce the shared library
clang $cFilenames $compilerFlags -o ../bin/lib$assembly.so $defines $includeFlags $linkerFlags