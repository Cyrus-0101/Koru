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

#if defined(_DEBUG)
    /**
     * @brief Debug messenger object used for receiving validation layer messages.
     *
     * Only present in debug builds. Used to register a callback function that receives
     * diagnostic, warning, and error messages from the Vulkan validation layers.
     */
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
} vulkan_context;