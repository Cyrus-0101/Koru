# Makefile.engine.windows.mak
# Purpose: Build the Koru engine shared library (.dll) on Windows using Clang and Make.
# This file defines how to compile and link the engine module as a DLL.

# Set current directory path with proper Windows backslash paths
DIR := $(subst /,\,${CURDIR})

# Output directories
BUILD_DIR := bin
OBJ_DIR := obj

# Shared library settings
ASSEMBLY := engine
EXTENSION := .dll

# Compiler flags
COMPILER_FLAGS := -g -MD -Werror=vla -fdeclspec -fPIC
# -g             : Enable debugging symbols
# -MD           : Generate dependency files (.d) for header tracking
# -Werror=vla   : Treat variable-length arrays as errors (enforces strict C99 compliance)
#                  This is useful for catching potential issues early.
# -fdeclspec     : Allow __declspec(dllexport/dllimport) usage
# -fPIC          : Position Independent Code (needed for DLLs even though Windows doesn't enforce it)

# Include paths
INCLUDE_FLAGS := -Iengine\src -I$(VULKAN_SDK)\include
# -I              : Adds include directories for header resolution

# Linker flags
LINKER_FLAGS := -g -shared -luser32 -lvulkan-1 -L$(VULKAN_SDK)\Lib -L$(OBJ_DIR)\engine
# -shared         : Tells clang to create a shared library
# -luser32       : Required for Windows message handling
# -lvulkan-1     : Links against Vulkan SDK's static import lib
# -L...           : Library search paths

# Defines
DEFINES := -D_DEBUG -DKEXPORT -D_CRT_SECURE_NO_WARNINGS
# -D_DEBUG        : Enables debug logging and asserts
# -DKEXPORT       : Marks engine functions as exported (for DLL)
# -D_CRT_SECURE_NO_WARNINGS : Suppresses Visual Studio warnings about unsafe CRT functions

# Recursive wildcard function
# Usage: $(call rwi
ldcard,<dir>,<pattern>)
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Source files
SRC_FILES := $(call rwildcard,$(ASSEMBLY)/,*.c)
# Recursively finds all C source files under 'engine/'

# Directories (for scaffolding)
DIRECTORIES := $(ASSEMBLY)\src $(subst $(DIR),,$(shell dir $(ASSEMBLY)\src /S /AD /B | findstr /i src))
# Used to create matching obj/ directory structure for Windows builds

# Object files
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o)
# Compiles each .c file into a .o object in obj/

# Targets
all: log-start scaffold compile link log-finish
# Default target — runs log start time, scaffold, compile, link and log finish time

.PHONY: log-start
log-start: # Logs build start time (EAT)
	@echo "===== Build started at $$(TZ=Africa/Nairobi date +'%Y-%m-%d %H:%M:%S.%3N %Z') ====="

.PHONY: log-finish
log-finish: # Logs build finish time (EAT)
	@echo "===== Build finished at $$(TZ=Africa/Nairobi date +'%Y-%m-%d %H:%M:%S.%3N %Z') ====="

.PHONY: scaffold
scaffold: # Creates folder structure needed for compilation
	@echo Scaffolding folder structure...
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addprefix $(OBJ_DIR), $(DIRECTORIES)) 2>NUL || cd .
	-@mkdir $(BUILD_DIR) 2>NUL || cd .
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES) # Links the engine shared library
	@echo Linking $(ASSEMBLY)...
	@clang $(OBJ_FILES) -o $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: compile
compile: # Compile all .c files into .o objects
	@echo Compiling...

.PHONY: clean
clean: # Clean up build artifacts
	if exist $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) del $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)
	rmdir /s /q $(OBJ_DIR)\$(ASSEMBLY)

# Rule to compile .c -> .o
$(OBJ_DIR)/%.c.o: %.c # Compile each .c file
	@echo   $<...
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)

-include $(OBJ_FILES:.o=.d)
# Include dependency files generated by -MD
# This allows Make to track header dependencies and rebuild as needed