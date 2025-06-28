# Makefile.engine.linux.mak
# Purpose: Build the Koru engine shared library (.so) on Linux using Clang and Make.
# This file defines how to compile and link the engine module as a shared library.

# Output directories
BUILD_DIR := bin
OBJ_DIR := obj
ASSEMBLY := engine
EXTENSION := .so

# Compiler flags
COMPILER_FLAGS := -g -MD -Werror=vla -fdeclspec -fPIC
# -g             : Enable debugging info (GDB)
# -fdeclspec     : Allow __declspec(dllexport/dllimport) for symbol visibility
# -fPIC          : Position Independent Code (required for shared libraries)

# Include paths
INCLUDE_FLAGS := -Iengine/src -I$(VULKAN_SDK)/include
# -I              : Adds include directories for header resolution

# Linker flags
LINKER_FLAGS := -g -shared -lvulkan -lxcb -lX11 -lX11-xcb -lxkbcommon -L$(VULKAN_SDK)/lib -L/usr/X11R6/lib
# -shared         : Builds a shared object (.so)
# -lvulkan        : Links against Vulkan SDK
# -lxcb           : XCB support for windowing
# -lX11          : X11 compatibility layer
# -lX11-xcb      : X11/XCB interop
# -lxkbcommon     : For keyboard handling
# -L...           : Library search paths

# Defines
DEFINES := -D_DEBUG -DKEXPORT
# -D_DEBUG        : Enables debug logging and asserts
# -DKEXPORT       : Marks engine functions as exported (for shared lib)

# Recursive wildcard function
# Usage: $(call rwildcard,<dir>,<pattern>)
# rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Source files
SRC_FILES := $(shell find $(ASSEMBLY) -name *.c)	 # Get all .c files
# Recursively finds all C source files under 'engine/'

# Directories (for scaffolding)
DIRECTORIES := $(shell find $(ASSEMBLY) -type d)
# Used to create matching obj/ directory structure

# Object files
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o)
# Compiles each .c file into a .o file in obj/

# Targets
all: scaffold compile link
# Default target — runs scaffold, compile, and link

.PHONY: scaffold
scaffold: # Creates the folder structure needed for compilation
	@echo Scaffolding folder structure...
	@mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRECTORIES))
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES) # Links the engine shared library
	@echo Linking $(ASSEMBLY)...
	@clang $(OBJ_FILES) -o $(BUILD_DIR)/lib$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)
# clang compiles object files into a shared library
# -shared ensures libengine.so is created

.PHONY: compile
compile: # Compiles all .c files into .o objects
	@echo Compiling...

.PHONY: clean
clean: # Cleans up build artifacts
	rm -rf $(BUILD_DIR)\$(ASSEMBLY)
	rm -rf $(OBJ_DIR)\$(ASSEMBLY)

# Rule to compile .c -> .o
$(OBJ_DIR)/%.c.o: %.c # Compile each .c file into an object
	@echo   $<...
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)
# Compiles individual C files with correct flags, includes, and defines