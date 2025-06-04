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

typedef struct vulkan_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

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
} vulkan_device;

/**
 * @struct vulkan_context
 * @brief Represents the global state of the Vulkan rendering context.
 *
 * This structure holds core Vulkan objects needed throughout the application's lifecycle.
 * It is intended to be initialized at startup and persisted for the lifetime of the renderer.
 */
typedef struct vulkan_context {
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
     */
    VkAllocationCallbacks* allocator;

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

    vulkan_device device;
} vulkan_context;