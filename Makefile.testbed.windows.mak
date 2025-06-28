# Makefile.testbed.windows.mak
# Purpose: Build the testbed application that uses the Koru engine.

# Set current directory path with proper Windows backslash paths
DIR := $(subst /,\,${CURDIR})

# Output directories
BUILD_DIR := bin
OBJ_DIR := obj

# Executable name and extension
ASSEMBLY := testbed
EXTENSION := .exe

# Compiler flags
COMPILER_FLAGS := -g -MD -Werror=vla -Wno-missing-braces -fdeclspec
# -g             : Debugging info for GDB or Visual Studio
# -Wno-missing-braces : Prevents warnings from struct initialization
# -fdeclspec     : Allows dllimport/dllexport macros

# Include paths
INCLUDE_FLAGS := -Iengine\src -Itestbed\src
# -I              : Adds include directories for header resolution

# Linker flags
LINKER_FLAGS := -g -lengine.lib -L$(OBJ_DIR)\engine -L$(BUILD_DIR)
# -lengine.lib    : Links with the engine shared library
# -L...           : Looks for libengine.lib in obj/engine or bin/
# -g               : Includes debug info for easier debugging

# Defines
DEFINES := -D_DEBUG -DKIMPORT
# -D_DEBUG        : Enables debug logging and asserts
# -DKIMPORT       : Treats engine functions as imported (from DLL)

# Recursive wildcard function (not used yet but available)
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Source files
SRC_FILES := $(call rwildcard,$(ASSEMBLY)/,*.c)
# Finds all C source files inside the testbed directory

# Directories (for scaffolding)
DIRECTORIES := $(ASSEMBLY)\src $(subst $(DIR),,$(shell dir $(ASSEMBLY)\src /S /AD /B | findstr /i src))
# Used to create obj/testbed directory structure

# Object files
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o)
# Maps each .c file to a corresponding .o in obj/

# Targets
all: scaffold compile link
# Default target — builds the testbed executable

.PHONY: scaffold
scaffold: # Create folder structure for object files
	@echo Scaffolding folder structure...
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addprefix $(OBJ_DIR), $(DIRECTORIES)) 2>NUL || cd .
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES) # Link the testbed executable
	@echo Linking $(ASSEMBLY)...
	@clang $(OBJ_FILES) -o $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: compile
compile: # Compile all .c files into .o
	@echo Compiling...

.PHONY: clean
clean: # Clean up build artifacts
	if exist $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) del $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)
	rmdir /s /q $(OBJ_DIR)\$(ASSEMBLY)

# Rule to compile .c -> .o
$(OBJ_DIR)/%.c.o: %.c # Compile each .c file
	@echo   $<...
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)