#pragma once

#include "defines.h"
#include "core/asserts.h"
#include <vulkan/vulkan.h>

/**
 * @file vulkan_types.inl
 * @brief Internal Vulkan-specific types used across the Koru engine.
 *
 * This file contains structures and typedefs specific to the Vulkan renderer implementation.
 * It should only be included in Vulkan-related source files.
 */

/**
 * @def VK_CHECK(expr)
 * @brief Checks if the result of a Vulkan API call is `VK_SUCCESS`.
 *
 * If the expression does not evaluate to `VK_SUCCESS`, this macro triggers an assertion failure,
 * which helps catch Vulkan errors early during development.
 *
 * @param expr A Vulkan API call that returns a VkResult.
 */
#define VK_CHECK(expr)               \
    {                                \
        KASSERT(expr == VK_SUCCESS); \
    }

/**
 * @struct vulkan_swapchain_support_info
 * @brief Stores information about a physical device's swapchain capabilities.
 *
 * This structure holds:
 * - Surface capabilities (min/max image count, resolution, etc.)
 * - Supported pixel formats
 * - Available presentation modes
 *
 * Used when selecting a suitable physical device and creating the swapchain.
 */
typedef struct vulkan_swapchain_support_info {
    /**
     * @brief Surface capabilities such as min/max image count, dimensions, and usage flags.
     */
    VkSurfaceCapabilitiesKHR capabilities;

    /**
     * @brief Number of available surface formats.
     */
    u32 format_count;

    /**
     * @brief Array of supported surface formats (e.g., R8G8B8A8_UNORM).
     */
    VkSurfaceFormatKHR* formats;

    /**
     * @brief Number of available present modes (e.g., FIFO, MAILBOX).
     */
    u32 present_mode_count;

    /**
     * @brief Array of supported presentation modes.
     */
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

/**
 * @struct vulkan_device
 * @brief Stores logical and physical device info along with queue data.
 *
 * Contains all necessary device-level state needed by the renderer.
 */
typedef struct vulkan_device {
    /**
     * @brief The physical GPU device selected for rendering.
     */
    VkPhysicalDevice physical_device;

    /**
     * @brief Logical device interface to the physical device.
     *
     * Created from the physical device and used for most Vulkan operations.
     */
    VkDevice logical_device;

    /**
     * @brief Swapchain support details for the current surface.
     */
    vulkan_swapchain_support_info swapchain_support;

    /**
     * @brief Queue family index for graphics operations.
     */
    i32 graphics_queue_index;

    /**
     * @brief Queue family index for presentation to the window surface.
     */
    i32 present_queue_index;

    /**
     * @brief Queue family index for transfer operations.
     */
    i32 transfer_queue_index;

    /**
     * @brief Graphics queue handle for submitting command buffers.
     */
    VkQueue graphics_queue;

    /**
     * @brief Presentation queue handle for displaying rendered images.
     */
    VkQueue present_queue;

    /**
     * @brief Transfer queue handle for memory copy operations.
     */
    VkQueue transfer_queue;

    /**
     * @brief Properties of the physical device (name, type, driver version, etc.).
     */
    VkPhysicalDeviceProperties properties;

    /**
     * @brief Features supported by the physical device (e.g., anisotropic filtering).
     */
    VkPhysicalDeviceFeatures features;

    /**
     * @brief Memory properties including heap sizes and locality flags.
     */
    VkPhysicalDeviceMemoryProperties memory;

    /**
     * @brief Format used for depth attachments (e.g., VK_FORMAT_D32_SFLOAT).
     */
    VkFormat depth_format;
} vulkan_device;

/**
 * @struct vulkan_image
 * @brief Represents a single Vulkan image resource with view and memory binding.
 *
 * Used for color/depth attachments and texture resources.
 */
typedef struct vulkan_image {
    /**
     * @brief Handle to the raw VkImage object.
     */
    VkImage handle;

    /**
     * @brief Device memory bound to this image.
     */
    VkDeviceMemory memory;

    /**
     * @brief Image view used to access the image in shaders/render passes.
     */
    VkImageView view;

    /**
     * @brief Width of the image in pixels.
     */
    u32 width;

    /**
     * @brief Height of the image in pixels.
     */
    u32 height;
} vulkan_image;

/**
 * @struct vulkan_swapchain
 * @brief Represents the application’s connection to the OS for rendering output.
 *
 * Holds the internal handle, image views, and depth attachment for rendering.
 */
typedef struct vulkan_swapchain {
    /**
     * @brief Format used for the swapchain images (e.g., B8G8R8A8_UNORM).
     */
    VkSurfaceFormatKHR image_format;

    /**
     * @brief Maximum number of frames that can be in flight at any time.
     */
    u8 max_frames_in_flight;

    /**
     * @brief Handle to the actual swapchain object.
     */
    VkSwapchainKHR handle;

    /**
     * @brief Total number of images in the swapchain.
     */
    u32 image_count;

    /**
     * @brief Array of VkImage handles provided by the swapchain.
     */
    VkImage* images;

    /**
     * @brief Array of VkImageView objects for accessing each swapchain image.
     */
    VkImageView* views;

    /**
     * @brief Depth attachment image used for rendering.
     */
    vulkan_image depth_attachment;
} vulkan_swapchain;

/**
 * @struct vulkan_context
 * @brief Represents the global state of the Vulkan rendering context.
 *
 * This structure holds core Vulkan objects needed throughout the application's lifecycle.
 * It is intended to be initialized at startup and persisted for the lifetime of the renderer.
 */
typedef struct vulkan_context {
    /**
     * @brief The current width of the framebuffer.
     */
    u32 framebuffer_width;

    /**
     * @brief The current height of the framebuffer.
     */
    u32 framebuffer_height;
    /**
     * @brief The VkInstance object that represents the connection between the application and the Vulkan library.
     *
     * This is the first object created when initializing Vulkan, and it manages per-application state.
     * It must be destroyed before the application exits using `vkDestroyInstance()`.
     */
    VkInstance instance;

    /**
     * @brief Optional custom allocator for Vulkan memory allocations.
     * If NULL, the default allocator is used.
     * Useful for tracking or debugging memory usage.
     */
    VkAllocationCallbacks* allocator;

    /**
     * @brief Window surface used for rendering output.
     *
     * Created from platform-specific code and used throughout the swapchain lifecycle.
     */
    VkSurfaceKHR surface;

#if defined(_DEBUG)
    /**
     * @brief Debug messenger object used for receiving validation layer messages.
     *
     * Only present in debug builds. Used to register a callback function that receives
     * diagnostic, warning, and error messages from the Vulkan validation layers.
     */
    VkDebugUtilsMessengerEXT debug_messenger;
#endif

    /**
     * @brief Device-specific state including queues, capabilities, and memory info.
     */
    vulkan_device device;

    /**
     * @brief Swapchain object used for rendering and presenting frames.
     */
    vulkan_swapchain swapchain;

    /**
     * @brief Index of the currently active image in the swapchain.
     */
    u32 image_index;

    /**
     * @brief Index of the current frame being rendered.
     *
     * Used to manage synchronization primitives per-frame.
     */
    u32 current_frame;

    /**
     * @brief Indicates whether the swapchain is currently being recreated.
     *
     * Prevents other operations while resizing or recreating the swapchain.
     */
    b8 recreating_swapchain;

    /**
     * @brief Function pointer for finding suitable memory index based on requirements.
     *
     * Used internally to find compatible memory types for buffer/image allocation.
     */
    i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
} vulkan_context;