# Koru 

[![Build Status](https://github.com/Cyrus-0101/Koru/workflows/CI/badge.svg )](https://github.com/Cyrus-0101/Koru/actions )

> *A lightweight, multi-platform game engine built in C using the Vulkan graphics API.*

Koru is designed to be fast, portable, and easy to extend — providing a low-level foundation for game development across platforms including Windows, Linux, and soon consoles and mobile devices.

---

## 🧠 What Is Koru?

Koru is a **C99-based game engine** focused on performance, portability, and modularity. It's ideal for developers who want fine-grained control over rendering, input, memory, and platform-specific functionality without the overhead of high-level abstractions.

### 🚀 Features:
- Written entirely in **C99** – minimal dependencies
- **Vulkan-based renderer** for high-performance graphics
- Cross-platform: **Windows, Linux, Mac** (with future plans for consoles and mobile)
- Modular architecture for **easy extension**
- Custom **memory allocators**, **logging**, **assertions**, and **data structures** etc

---

## 🖼 Architecture

The high-level architecture of Koru is divided into four main layers:

### 1. **Platform Layer**

Handles OS-specific functionality:

- **Windowing**: Creates and manages windows on Windows/Linux/Mac.
- **Input**: Manages keyboard, mouse, and gamepad input.
- **File I/O**: Provides cross-platform file system access.
- **Console Output**: Handles logging and debugging output.
- **Memory Management**: Allocates and frees memory efficiently.
- **Renderer API Extensions**: Integrates with Vulkan for rendering.

### 2. **Core**

Provides fundamental utilities and services:

- **Logger**: Logs messages for debugging and monitoring.
- **Assertions**: Ensures correctness during development.
- **Data Structures**: Efficient containers like dynamic arrays, linked lists, etc.
- **Memory Allocators**: Custom allocators for optimized memory usage.
- **Math Library**: Vector, matrix, and quaternion operations.
- **Parsers**: Parses formats like XML, CSV, JSON, etc.
- **Engine Configuration**: Loads and manages engine settings.
- **Profiling**: Measures performance bottlenecks.
- **Async File I/O**: Asynchronous file operations for non-blocking I/O.
- **Localization**: Supports internationalization.
- **String Library**: String manipulation utilities.
- **Random Number Generator (RNG)**: Generates random numbers for simulations and AI.

### 3. **Resource Management**

Manages various assets:

- **Images**: Texture loading and caching.
- **Materials**: Shader and lighting configurations.
- **Meshes**: 3D model loading and processing.
- **Animations**: Skeletal animations and timeline systems.
- **World Maps**: Terrain and level data management.

### 4. **Application Layer**

Contains higher-level systems:

- **Renderer Front End**: GUI, camera management, post-processing effects.
- **Renderer Back End**: Scene graph, materials, shaders, meshes, textures.
- **Audio**: 2D/3D audio playback, effects, and spatial audio.
- **Others**: Animation/timelines, event system, ECS, state machines, collision/physics.

---

## 📋 Feature List

Here’s a summary of the features to be implemented in v0.1:

### Core Features

- **Lightweight build system**: Simple scripts for compiling on Windows and Linux.
- **Low-level utilities**:
  - Dynamic arrays
  - String handling
  - Memory allocators
  - Profiling tools
  - Assertions
  - Logging
- **Platform layer**:
  - Window creation and management
  - Input handling (keyboard, mouse, gamepad)
  - File I/O
  - Console output
- **Renderer/API Abstraction Layer**: Uses Vulkan for cross-platform rendering.
- **Memory Management**: Custom allocators for efficient memory use.
- **Scenegraph/ECS**: Hierarchical scene management and entity-component-system architecture.
- **Profiling/Debugging utilities**: Tools for measuring performance and identifying bottlenecks.
- **"Scripting" support via hot reloading**: Experimental support for scripting through hot-reloading mechanisms.

### Planned Features

- Mobile and console support
- Full audio subsystem
- Physics and collision detection
- Advanced animation system
- Improved asset pipeline

---

## 🎨 Visuals

Check out the diagrams included in the `assets/` folder to understand how everything fits together:

| Diagram | Description |
|--------|-------------|
| ![Architecture](assets/Koru%20Engine%20Architecture.png) | High-level overview of system modules and interactions |
| ![Project Structure](assets/Koru%20Project%20Structure.png) | File structure and module organization |
| ![Feature List v0.1](assets/Koru%20v0.1%20Feature%20List.png) | Summary of current capabilities and roadmap |

---

## 📁 Project Structure

```
.
├── assets
│   ├── Koru Engine Architecture.png        # High-level system architecture diagram
│   ├── Koru Project Structure.png          # Visual representation of file organization
│   └── Koru v0.1 Feature List.png          # Summary of current features and roadmap
├── bin
│   ├── libengine.so                        # Compiled engine shared library (Linux)
│   └── testbed                             # Testbed application binary (Linux)
├── build-all.bat                           # Windows: Runs all builds sequentially
├── build-all.sh                            # Linux: Runs all builds sequentially
├── Doxyfile                                # Configuration for generating API documentation with Doxygen
├── engine
│   ├── build.bat                           # Windows: Builds engine DLL
│   ├── build.sh                            # Linux: Builds engine shared object (.so)
│   └── src
│       ├── containers
│       │   ├── darray.c                    # Implementation of dynamic array container
│       │   └── darray.h                    # Public interface for dynamic arrays
│       ├── core
│       │   ├── application.c               # Application lifecycle implementation (init/run/shutdown)
│       │   ├── application.h               # Public interface for the application system
│       │   ├── asserts.h                   # Custom assertion macros and debugging utilities
│       │   ├── event.c                     # Event system implementation (register/unregister/fire)
│       │   ├── event.h                     # Public interface for event handling system
│       │   ├── kmemory.c                   # Memory allocation system with tagging support
│       │   ├── kmemory.h                   # Interface for tagged memory management
│       │   ├── logger.c                    # Logging system with color-coded output
│       │   └── logger.h                   # Public interface for logging module
│       ├── defines.h                       # Common type definitions, macros, and platform detection
│       ├── entry.h                         # Entry point interface; declares create_game()
│       ├── game_types.h                    # Game-related types used across engine and game logic
│       └── platform
│           ├── platform.h                 # Platform abstraction layer interface
│           ├── platform_linux.c           # Linux-specific implementation of platform functions
│           └── platform_win32.c           # Windows-specific implementation of platform functions
├── LICENSE                                 # MIT License file
├── README.md                               # This file - main project overview and guide
├── test
│   └── platform                           # Unit tests for platform module
├── testbed
    ├── build.bat                           # Windows: Builds testbed application
    ├── build.sh                            # Linux: Builds testbed application
    └── src
        ├── entry.c                         # Entry point for testbed app; implements create_game()
        ├── game.c                          # Testbed game logic (initialize/update/render stubs)
        └── game.h                          # Interface for testbed game implementation
```

---

## ⚙️ Build Scripts/Instructions

Check tasks.json for sequence:

| File                | Purpose                                              | Notes                         |
| ------------------- | ---------------------------------------------------- | ----------------------------- |
| `engine/build.sh`   | Builds engine `.so` (shared lib) on Linux            | Uses Vulkan/XCB/X11           |
| `engine/build.bat`  | Builds engine `.dll` on Windows                      | Links with Vulkan SDK         |
| `testbed/build.sh`  | Builds testbed app using engine `.so` on Linux       | Sets RPATH for easy loading   |
| `testbed/build.bat` | Builds testbed `.exe` using engine `.dll` on Windows | Links `.lib` import library   |
| `build-all.sh`      | Runs all Linux builds sequentially                   | Checks for errors             |
| `build-all.bat`     | Runs all Windows builds sequentially                 | Same as above but for Windows |

### Linux

Make sure Clang and the Vulkan SDK are installed.
Alternatively install VSCode and run the debugger (Commonly F5, and run your engine)

```bash
# Give execution permissions
chmod +x build-all.sh

# Run the build
./build-all.sh
```

The output will be placed in `bin/`:

- `libengine.so` – The engine shared library
- `testbed` – The testbed executable

### Windows

Ensure:

- Clang is available
- Vulkan SDK is installed and `VULKAN_SDK` environment variable is set
- VS Code makes it easy to run the tasks in the .vscode/tasks.json file.

Then run:

```cmd
build-all.bat
```

Output files:

- `bin\engine.dll`
- `bin\testbed.exe`

---

## 🔮 Roadmap

Plans to expand Koru include:

- ✅ **Rendering Layer**

  - [ ] Vulkan renderer initialization
  - [ ] Swapchain setup
  - [ ] Command buffers, pipelines, shaders

- ✅ **Input System**

  - [ ] Keyboard/mouse/gamepad support
  - [ ] Input mapping and event system

- ✅ **Asset Management**

  - [ ] Texture loader
  - [ ] Mesh format and loader
  - [ ] Material system

- ✅ **Audio System**

  - [ ] Basic sound playback
  - [ ] Cross-platform audio backend

- ✅ **Multi-Platform Support**

  - [ ] Console (PlayStation, Xbox, Switch)
  - [ ] Mobile (Android/iOS)

- ✅ **Editor Tools**
  - [ ] Simple scene editor
  - [ ] Asset pipeline tools

---

## 💡 Want to Help?

Contributions are welcome! Whether you're interested in:

- Writing documentation
- Implementing new features
- Fixing bugs
- Improving tooling or build scripts

Feel free to open an issue or PR!

---

## 📄 License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.

```

## 🚀 Next Steps

TO-DO: Add:

- ✅ A **Getting Started Guide** (`docs/getting-started.md`)
- ✅ A **Contribution Guide** (`CONTRIBUTING.md`)
- ✅ A **Code of Conduct** (`CODE_OF_CONDUCT.md`)
- ✅ GitHub Actions for CI builds (optional)

```
