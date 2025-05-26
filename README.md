# Koru

A lightweight, multi-platform game engine built in C using the Vulkan graphics API.

Koru is designed to be fast, portable, and easy to extend — providing a low-level foundation for game development across platforms including Windows, Linux, and soon consoles and mobile devices.

---

## 🧠 Overview

Koru is designed to be:

- **Portable**: Supports Windows, Linux, and Mac (with plans for consoles and mobile platforms).
- **Modular**: Built with a clean separation of concerns, allowing easy extension and customization.
- **Performance-Oriented**: Optimized for speed and low-level control, leveraging Vulkan for high-performance rendering.

It serves as a foundation for creating games, tools, or applications that need direct access to hardware and APIs without the overhead of higher-level abstractions, fine-grained control over rendering, input, memory, and platform-specific functionality.

All code is written in **C99**, making it ideal for performance-critical applications while keeping dependencies minimal.

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

## 🖼 Visual Assets

Take a look at the included diagrams in the `assets/` folder:

| File                           | Description                                                  |
| ------------------------------ | ------------------------------------------------------------ |
| `Koru Engine Architecture.png` | High-level overview of system layers and module interactions |
| `Koru Project Structure.png`   | File structure and organization of source modules            |
| `Koru v0.1 Feature List.png`   | Summary of current capabilities and future goals             |

---

## 📁 Project Structure

```
.
├── assets/              # Diagrams showing architecture and feature list
├── bin/                 # Compiled binaries (engine shared lib + testbed app)
├── build-all.bat        # Build script for Windows (builds all modules)
├── build-all.sh         # Build script for Linux (builds all modules)
├── engine/              # Core engine code and platform abstraction
│   ├── build.bat        # Windows build script for engine (.dll)
│   ├── build.sh         # Linux build script for engine (.so)
│   └── src/
│       ├── core/        # Core utilities: logging, asserts, memory
│       ├── defines.h    # Common definitions and macros
│       └── platform/    # Platform-specific code (Linux/Windows)
├── LICENSE              # MIT License
├── README.md            # This file
└── testbed/             # Example application that uses the engine
    ├── build.bat        # Windows build script for testbed
    ├── build.sh         # Linux build script for testbed
    └── src/
        └── main.c       # Entry point for testbed application
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
