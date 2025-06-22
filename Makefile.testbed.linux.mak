# Makefile.testbed.linux.mak
# Purpose: Build the testbed application that uses the Koru engine.

# Output directories
BUILD_DIR := bin
OBJ_DIR := obj

# Binary name and extension
ASSEMBLY := testbed
EXTENSION := 

# Compiler flags
COMPILER_FLAGS := -g -fdeclspec -fPIC
# -g             : Debug symbols for GDB
# -fdeclspec     : Allows dllimport/dlexport syntax
# -fPIC          : Required for position-independent code (even for executables)

# Include paths
INCLUDE_FLAGS := -Iengine/src -I$(VULKAN_SDK)/include
# Includes headers from engine and Vulkan SDK

# Linker flags
LINKER_FLAGS := -L./$(BUILD_DIR)/ -lengine -Wl,-rpath,.
# -L...           : Looks for libengine.so in bin/
# -lengine        : Links with libengine.so
# -Wl,-rpath,.   : Tells the binary where to look for shared libraries at runtime

# Defines
DEFINES := -D_DEBUG -DKIMPORT
# -D_DEBUG        : Enables debug logging and asserts
# -DKIMPORT       : Treats engine functions as imported (from .so)

# Recursive wildcard function (not used yet but available)
# rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Source files
SRC_FILES := $(shell find $(ASSEMBLY) -name *.c)
# Finds all C files inside the testbed directory

# Directories (for scaffolding)
DIRECTORIES := $(shell find $(ASSEMBLY) -type d)
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
	@mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRECTORIES))
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES) # Link the testbed executable
	@echo Linking $(ASSEMBLY)...
	clang $(OBJ_FILES) -o $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)
# Links testbed main.c with libengine.so and other dependencies

.PHONY: compile
compile: # Compile all .c files into .o
	@echo Compiling...

.PHONY: clean
clean: # Clean up build artifacts
	rm -rf $(BUILD_DIR)/$(ASSEMBLY)
	rm -rf $(OBJ_DIR)/$(ASSEMBLY)

# Rule to compile .c -> .o
$(OBJ_DIR)/%.c.o: %.c # Compile each .c file
	@echo   $<...
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)
# Compiles individual C files with debug info and Vulkan includes