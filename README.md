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
│   ├── Koru Engine Architecture.png
│   ├── Koru Project Structure.png
│   ├── Koru v0.1 Feature List.png
│   └── shaders
│       ├── Builtin.ObjectShader.frag.glsl
│       └── Builtin.ObjectShader.vert.glsl
├── bin
│   ├── assets
│   │   ├── Koru Engine Architecture.png
│   │   ├── Koru Project Structure.png
│   │   ├── Koru v0.1 Feature List.png
│   │   └── shaders
│   │       ├── Builtin.ObjectShader.frag.glsl
│   │       ├── Builtin.ObjectShader.frag.spv
│   │       ├── Builtin.ObjectShader.vert.glsl
│   │       └── Builtin.ObjectShader.vert.spv
│   ├── console.log
│   ├── libengine.so
│   ├── testbed
│   └── tests
├── build-all.bat
├── build-all.sh
├── clean-all.bat
├── clean-all.sh
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
│       │   ├── clock.c
│       │   ├── clock.h
│       │   ├── event.c                     # Event system implementation (register/unregister/fire)
│       │   ├── event.h                     # Public interface for event handling system
│       │   ├── input.c
│       │   ├── input.h
│       │   ├── kmemory.c                   # Memory allocation system with tagging support
│       │   ├── kmemory.h                   # Interface for tagged memory management
│       │   ├── kstring.c
│       │   ├── kstring.h
│       │   ├── logger.c                    # Logging system with color-coded output
│       │   └── logger.h                    # Public interface for logging module
│       ├── defines.h                       # Common type definitions, macros, and platform detection
│       ├── entry.h                         # Entry point interface; declares create_game()
│       ├── game_types.h                    # Game-related types used across engine and game logic
│       ├── math
│       │   ├── kmath.c
│       │   ├── kmath.h
│       │   └── math_types.h
│       ├── memory
│       │   ├── linear_allocator.c
│       │   └── linear_allocator.h
│       ├── platform
│       │   ├── filesystem.c
│       │   ├── filesystem.h
│       │   ├── platform_android.c
│       |   ├── platform.h                 # Platform abstraction layer interface
│       │   ├── platform_ios.c
│       │   ├── platform_linux.c           # Linux-specific implementation of platform functions
│       │   ├── platform_macos.c
│       │   └── platform_win32.c           # Windows-specific implementation of platform functions
│       └── renderer
│           ├── renderer_backend.c
│           ├── renderer_backend.h
│           ├── renderer_frontend.c
│           ├── renderer_frontend.h
│           ├── renderer_types.inl
│           └── vulkan
│               ├── shaders
│               │   ├── vulkan_object_shader.c
│               │   └── vulkan_object_shader.h
│               ├── vulkan_backend.c
│               ├── vulkan_backend.h
│               ├── vulkan_buffer.c
│               ├── vulkan_buffer.h
│               ├── vulkan_command_buffer.c
│               ├── vulkan_command_buffer.h
│               ├── vulkan_device.c
│               ├── vulkan_device.h
│               ├── vulkan_fence.c
│               ├── vulkan_fence.h
│               ├── vulkan_framebuffer.c
│               ├── vulkan_framebuffer.h
│               ├── vulkan_image.c
│               ├── vulkan_image.h
│               ├── vulkan_pipeline.c
│               ├── vulkan_pipeline.h
│               ├── vulkan_platform.h
│               ├── vulkan_renderpass.c
│               ├── vulkan_renderpass.h
│               ├── vulkan_shader_utils.c
│               ├── vulkan_shader_utils.h
│               ├── vulkan_swapchain.c
│               ├── vulkan_swapchain.h
│               ├── vulkan_types.inl
│               ├── vulkan_utils.c
│               └── vulkan_utils.h
├── LICENSE                                 # MIT License file
├── Makefile.engine.linux.mak
├── Makefile.engine.windows.mak
├── Makefile.testbed.linux.mak
├── Makefile.testbed.windows.mak
├── post-build.bat
├── post-build.sh
├── README.md                               # This file - main project overview and guide
├── testbed
|   ├── build.bat                           # Windows: Builds testbed application
|   ├── build.sh                            # Linux: Builds testbed application
|   └── src
|       ├── entry.c                         # Entry point for testbed app; implements create_game()
|       ├── game.c                          # Testbed game logic (initialize/update/render stubs)
|       └── game.h                          # Interface for testbed game implementation
└── tests
    ├── build.bat
    ├── build.sh
    └── src
        ├── expect.h
        ├── main.c
        ├── memory
        │   ├── linear_allocator_tests.c
        │   └── linear_allocator_tests.h
        ├── test_manager.c
        └── test_manager.h

37 directories, 176 files   
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
Alternatively install VSCode and run the debugger (Commonly F5, and run the engine)

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

## Building Docs

To generate our Doxygen documentation, run:

```sh
    doxygen Doxyfile
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

---

# 🎮 Koru Engine – Rendering System (Vulkan)

> *A deep dive into how Koru sets up its Vulkan-based rendering system — from instance creation to frame submission.*

Koru is a **low-level game engine** written in C99 that uses the **Vulkan graphics API** for rendering. This document explains the **entire rendering pipeline setup** used in Koru, including:
- Instance creation
- Physical device selection
- Logical device creation
- Surface & swapchain setup
- Render passes
- Framebuffers
- Command buffers

This guide assumes some basic knowledge of C and Vulkan concepts. If you're new to Vulkan, this will help understand what’s happening behind the scenes as you run the testbed.

---

## 🧠 Why Use Vulkan?

Vulkan gives developers **explicit control over GPU operations**. It allows fine-grained access to queues, memory, and rendering pipelines — which is great for performance, but comes at the cost of complexity.

Koru abstracts much of that complexity using a **modular architecture**, allowing you to build a rendering system step-by-step without needing to manage everything manually every time.

---

## 📦 High-Level Architecture (Render Layer)

```
+------------------+
|   Application    |
|     Logic        |
|  (game.c)        |
+------------------+
         |
         v
+------------------+
|   Renderer       |
|   Frontend       |
| (renderer_frontend.h/c) |
+------------------+
         |
         v
+------------------+
|   Platform       |
|   Abstraction    |
| (platform_linux.c)|
+------------------+
         |
         v
+------------------+
|   Vulkan         |
|   Device         |
| (vulkan_device.c/h) |
+------------------+
         |
         v
+------------------+
|   Swapchain      |
|   Management     |
| (vulkan_swapchain.c/h) |
+------------------+
         |
         v
+------------------+
|   Render Pass /  |
|   Framebuffer    |
| (vulkan_renderpass.c/h) |
+------------------+
         |
         v
+------------------+
|   Image          |
|   Management     |
| (vulkan_image.c/h) |
+------------------+
         |
         v
+------------------+
|   Command Buffer |
|   Submission     |
| (renderer_frontend.c) |
+------------------+
```

This shows how each module fits into the overall rendering flow.

---

## 🛠️ The Full Rendering Setup Process

Let’s walk through **each stage of the rendering setup process**, with real code examples from the engine.

---

### 1. 🧱 Create Vulkan Instance

The first thing to do is creating a **VkInstance** — this connects the app to the Vulkan library.

```c
// From vulkan_renderer_backend_initialize()
VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
app_info.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);
app_info.pApplicationName = application_name;
app_info.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
app_info.pEngineName = "Koru Engine";
app_info.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);

VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
create_info.pApplicationInfo = &app_info;

const char** required_extensions = darray_create(const char*);
darray_push(required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME);
platform_get_required_extension_names(&required_extensions);

create_info.enabledExtensionCount = darray_length(required_extensions);
create_info.ppEnabledExtensionNames = required_extensions;

// Enable validation layers if _DEBUG is defined
#if defined(_DEBUG)
    create_info.enabledLayerCount = required_validation_layer_count;
    create_info.ppEnabledLayerNames = required_validation_layer_names;
#endif

VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));
```

✅ This creates the **main connection to Vulkan**, enabling extensions and validation layers.

---

### 2. 🖥 Create Window Surface

Next, to create a **surface** — this is what connects the window to the GPU.

In Linux:

```c
// platform_linux.c
xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(...);
state->wm_delete_win = wm_delete_reply->atom;

// Create surface
VkXcbSurfaceCreateInfoKHR surface_create_info = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
surface_create_info.connection = state->connection;
surface_create_info.window = state->window;

vkCreateXcbSurfaceKHR(context->instance, &surface_create_info, context->allocator, &context->surface);
```

In Windows: The engine uses `vkCreateWin32SurfaceKHR()` instead.

✅ A surface is essential because it defines where rendered frames go usually the screen or a window.

---

### 3. 🔍 Query Physical Devices

Once an instance and a surface are ready, query available GPUs:

```c
// vulkan_device.c
u32 physical_device_count = 0;
VK_CHECK(vkEnumeratePhysicalDevices(context->device.physical_device, &physical_device_count, 0));

VkPhysicalDevice physical_devices[physical_device_count];
VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices));
```

Then select one that meets requirements:

```c
for (u32 i = 0; i < physical_device_count; ++i) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

    // Check queue family support
    vulkan_physical_device_queue_family_info queue_info = {};
    vulkan_swapchain_support_info swapchain_support = {};

    b8 result = physical_device_meets_requirements(
        physical_devices[i],
        context->surface,
        &properties,
        &features,
        &requirements,
        &queue_info,
        &swapchain_support);
}
```

🔍 Requirements include:
- Graphics/Compute/Present/Transfer queues
- Extensions like `VK_KHR_SWAPCHAIN_EXTENSION_NAME`
- Features like `samplerAnisotropy`
- Prefer discrete GPU (optional)

---

### 4. 🔄 Create Logical Device

After selecting a suitable physical device, create a **logical device** to interface with the GPU.

```c
// vulkan_device.c
b8 present_shares_graphics_queue = (context->device.graphics_queue_index == context->device.present_queue_index);
b8 transfer_shares_graphics_queue = (context->device.graphics_queue_index == context->device.transfer_queue_index);

u32 index_count = 1;
if (!present_shares_graphics_queue) index_count++;
if (!transfer_shares_graphics_queue) index_count++;

u32 indices[index_count];
indices[0] = context->device.graphics_queue_index;
...
```

Define the queues as needed:

```c
VkDeviceQueueCreateInfo queue_create_infos[index_count];
for (u32 i = 0; i < index_count; ++i) {
    queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[i].queueFamilyIndex = indices[i];
    queue_create_infos[i].queueCount = 1;
    f32 queue_priority = 1.0f;
    queue_create_infos[i].pQueuePriorities = &queue_priority;
}
```

Finally, create the logical device:

```c
VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
device_create_info.queueCreateInfoCount = index_count;
device_create_info.pQueueCreateInfos = queue_create_infos;
device_create_info.pEnabledFeatures = &device_features;

VK_CHECK(vkCreateDevice(context->device.physical_device, &device_create_info, context->allocator, &context->device.logical_device));
```

✅ This creates a way to submit commands to the GPU.

---

### 5. ⏱ Get Queues

Once the device is created, get handles to the actual queues:

```c
vkGetDeviceQueue(context->device.logical_device, context->device.graphics_queue_index, 0, &context->device.graphics_queue);
vkGetDeviceQueue(context->device.logical_device, context->device.present_queue_index, 0, &context->device.present_queue);
```

These are used later to:
- Submit draw commands (`graphics_queue`)
- Present frames (`present_queue`)
- Copy data (`transfer_queue`)

---

### 6. 🔄 Query Swapchain Support

Before creating a swapchain, check what formats and modes are supported:

```c
// vulkan_device.c
void vulkan_device_query_swapchain_support(...) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &out_support->capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &out_support->format_count, 0);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &out_support->present_mode_count, 0);
}
```

This helps choose the best image format and presentation mode.

---

### 7. 🔁 Create Swapchain

Now create the swapchain based on what the device supports:

```c
// vulkan_swapchain.c
VkSwapchainCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
create_info.surface = context->surface;
create_info.minImageCount = image_count;
create_info.imageFormat = swapchain->image_format.format;
create_info.imageExtent = swapchain_extent;
create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

if (context->device.graphics_queue_index != context->device.present_queue_index) {
    u32 queueFamilyIndices[] = {
        (u32)context->device.graphics_queue_index,
        (u32)context->device.present_queue_index};

    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queueFamilyIndices;
} else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
}

VK_CHECK(vkCreateSwapchainKHR(context->device.logical_device, &create_info, context->allocator, &swapchain->handle));
```

🔄 The swapchain acts like a rotating carousel of images:
- One is being drawn
- One is being displayed
- One is waiting to be used next

This avoids tearing and ensures smooth rendering.

---

### 8. 🖼 Create Image Views

Each image in the swapchain needs a **view** so the GPU can interpret it:

```c
for (u32 i = 0; i < swapchain->image_count; ++i) {
    VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_info.image = swapchain->images[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = swapchain->image_format.format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.layerCount = 1;

    VK_CHECK(vkCreateImageView(context->device.logical_device, &view_info, context->allocator, &swapchain->views[i]));
}
```

🖼 Each image view wraps a raw `VkImage`, defining how it's accessed (e.g., color attachment, depth buffer).

---

### 9. 🧊 Detect Depth Format

For depth testing, a compatible depth format is needed:

```c
b8 vulkan_device_detect_depth_format(vulkan_device* device) {
    const u64 candidate_count = 3;
    VkFormat candidates[candidate_count] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT};

    for (u64 i = 0; i < candidate_count; ++i) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i], &props);

        if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
            device->depth_format = candidates[i];
            return True;
        }
    }

    KERROR("No supported depth format found.");
    return False;
}
```

🧃 Once selected, create a depth/stencil image and its view.

---

### 10. 🎨 Create Render Pass

The render pass defines how rendering works — what attachments to use, how they behave, and what subpasses exist.

```c
// vulkan_renderpass.c
VkAttachmentDescription attachments[2] = {};
attachments[0].format = swapchain->image_format.format;
attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

attachments[1].format = context->device.depth_format;
attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

VkSubpassDescription subpass = {};
subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
subpass.colorAttachmentCount = 1;
subpass.pColorAttachments = &color_ref;
subpass.pDepthStencilAttachment = &depth_stencil_ref;

VkRenderPassCreateInfo info = {};
info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
info.attachmentCount = 2;
info.pAttachments = attachments;
info.subpassCount = 1;
info.pSubpasses = &subpass;

VK_CHECK(vkCreateRenderPass(context->device.logical_device, &info, context->allocator, &renderpass->handle));
```

🎨 The render pass is like a **blueprint** — it tells the GPU:
- What kind of data it will write to
- How to handle clearing and storing it
- What layout transitions happen between steps

---

### 11. 🖼 Create Framebuffers

Framebuffers combine images and render passes:

```c
VkFramebufferCreateInfo fb_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
fb_info.renderPass = renderpass->handle;
fb_info.attachmentCount = 2;
fb_info.pAttachments = attachments;
fb_info.width = width;
fb_info.height = height;
fb_info.layers = 1;

VK_CHECK(vkCreateFramebuffer(device, &fb_info, 0, &framebuffer));
```

🖼 Each framebuffer links:
- A color image from the swapchain
- A depth/stencil image
- The render pass that defines how to use them

---

### 12. 🗂 Command Buffers and Submission

Command buffers store drawing commands:

```c
// Begin command recording
VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
vkBeginCommandBuffer(command_buffer, &begin_info);

// Record draw commands here...

vkEndCommandBuffer(command_buffer);
```

Then submit to the GPU:

```c
VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
submit_info.waitSemaphoreCount = 1;
submit_info.pWaitSemaphores = &image_available_semaphore;
submit_info.commandBufferCount = 1;
submit_info.pCommandBuffers = &command_buffer;

VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit_info, fence));
```

And finally present:

```c
VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
present_info.swapchainCount = 1;
present_info.pSwapchains = &swapchain->handle;
present_info.pImageIndices = &image_index;

VkResult result = vkQueuePresentKHR(present_queue, &present_info);
```

---

## 🧩 Diagram: Full Rendering Flow

```
+-------------------+
|   Application     |
|   Entry Point     |
| (application_run())|
+-------------------+
           |
           v
+-------------------+
|   Create Instance |
| (vkCreateInstance)|
+-------------------+
           |
           v
+-------------------+
|   Create Surface  |
| (vkCreateXcbSurfaceKHR) |
+-------------------+
           |
           v
+-------------------+
| Enumerate Physical|
| Devices           |
| (vkEnumeratePhysicalDevices) |
+-------------------+
           |
           v
+-------------------+
| Select Suitable   |
| Physical Device   |
| (select_physical_device) |
+-------------------+
           |
           v
+-------------------+
| Create Logical    |
| Device            |
| (vkCreateDevice)  |
+-------------------+
           |
           v
+-------------------+
| Get Queue Handles |
| (vkGetDeviceQueue) |
+-------------------+
           |
           v
+-------------------+
| Query Swapchain   |
| Support           |
| (vkGetPhysicalDeviceSurface*) |
+-------------------+
           |
           v
+-------------------+
| Create Swapchain  |
| (vkCreateSwapchainKHR) |
+-------------------+
           |
           v
+-------------------+
| Create Image Views|
| (vkCreateImageView) |
+-------------------+
           |
           v
+-------------------+
| Detect Depth      |
| Format            |
| (vulkan_device_detect_depth_format) |
+-------------------+
           |
           v
+-------------------+
| Create Depth Image|
| (vulkan_image_create) |
+-------------------+
           |
           v
+-------------------+
| Create Render Pass|
| (vkCreateRenderPass) |
+-------------------+
           |
           v
+-------------------+
| Create Framebuffer|
| (vkCreateFramebuffer) |
+-------------------+
           |
           v
+-------------------+
| Begin Main Loop   |
| (vkAcquireNextImageKHR) |
+-------------------+
           |
           v
+-------------------+
| Begin Render Pass |
| (vkCmdBeginRenderPass) |
+-------------------+
           |
           v
+-------------------+
| Record Draw       |
| Commands          |
+-------------------+
           |
           v
+-------------------+
| End Render Pass   |
| (vkCmdEndRenderPass) |
+-------------------+
           |
           v
+-------------------+
| Submit Command    |
| Buffer to GPU     |
| (vkQueueSubmit)   |
+-------------------+
           |
           v
+-------------------+
| Present Frame     |
| (vkQueuePresentKHR) |
+-------------------+
```

This is the full **rendering loop** that happens every frame.

---

## 🧰 Key Structures Used

### `vulkan_context`
Tracks global Vulkan state:
```c
typedef struct vulkan_context {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debug_messenger;
    vulkan_device device;
    vulkan_swapchain swapchain;
    u32 image_index;
    u32 current_frame;
    b8 recreating_swapchain;
    i32 (*find_memory_index)(...);
} vulkan_context;
```

### `vulkan_device`
Holds physical/logical device and queue info:
```c
typedef struct vulkan_device {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    vulkan_swapchain_support_info swapchain_support;
    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
    VkFormat depth_format;
} vulkan_device;
```

### `vulkan_swapchain`
Manages images and depth buffer:
```c
typedef struct vulkan_swapchain {
    VkSurfaceFormatKHR image_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR handle;
    u32 image_count;
    VkImage* images;
    VkImageView* views;
    vulkan_image depth_attachment;
} vulkan_swapchain;
```

### `vulkan_renderpass`
Defines clear values and viewport settings:
```c
typedef struct vulkan_renderpass {
    VkRenderPass handle;
    f32 clear_r, clear_g, clear_b, clear_a;
    f32 clear_depth;
    u32 clear_stencil;
    f32 viewport_x, viewport_y, viewport_w, viewport_h;
    f32 scissor_x, scissor_y, scissor_w, scissor_h;
} vulkan_renderpass;
```

---

## 💡 Why All This Is Needed

Vulkan is explicit by design. Every operation must be described precisely. Here's why each part matters:

| Component | Purpose |
|----------|---------|
| **Instance** | Connects app to Vulkan driver |
| **Surface** | Links window to GPU output |
| **Physical Device** | Represents the actual GPU |
| **Logical Device** | Interface to the GPU |
| **Queues** | Workers for graphics/present/transfer |
| **Swapchain** | Manages sequence of renderable images |
| **Images** | Raw GPU memory to draw into |
| **Image Views** | Define how to read/write those images |
| **Render Pass** | Defines rendering rules (clear/store ops, layout) |
| **Framebuffers** | Combine images into something you can draw into |
| **Command Buffers** | Store GPU commands to execute later |

---

## 🧱 Summary Table

| Module | Responsibility |
|--------|----------------|
| `vulkan_renderer_backend_initialize()` | Creates instance, surface, and starts device setup |
| `vulkan_device_create()` | Selects physical device, creates logical device |
| `vulkan_device_query_swapchain_support()` | Queries device capabilities for swapchain |
| `vulkan_swapchain_create()` | Initializes the swapchain and image views |
| `vulkan_device_detect_depth_format()` | Finds a usable depth format |
| `vulkan_renderpass_create()` | Sets up how we render |
| `vulkan_command_buffer_begin()` | Starts recording draw commands |
| `vulkan_swapchain_present()` | Presents the final image to the screen |

---

## 🧪 Bonus: Sample Code for Acquiring an Image

```c
b8 vulkan_swapchain_acquire_next_image_index(...) {
    VkResult result = vkAcquireNextImageKHR(
        context->device.logical_device,
        swapchain->handle,
        timeout_ns,
        image_available_semaphore,
        fence,
        out_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vulkan_swapchain_recreate(context, width, height, swapchain);
        return False;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        KFATAL("Failed to acquire swapchain image!");
        return False;
    }

    return True;
}
```

This function gets the next image from the swapchain — and triggers recreation if needed.

---

## 🚫 Common Errors and Fixes

### ❌ `VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_EXT`: No cache file

Not critical — just means the shader cache doesn't exist yet.

---
