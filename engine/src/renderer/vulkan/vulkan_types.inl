#pragma once

#include "defines.h"

#include <vulkan/vulkan.h>

/**
 * @file vulkan_types.inl
 * @brief Internal Vulkan-specific types used across the Koru engine.
 *
 * This file contains structures and typedefs specific to the Vulkan renderer implementation.
 * It should only be included in Vulkan-related source files.
 */

/**
 * @struct vulkan_context
 * @brief Represents the global state of the Vulkan rendering context.
 */
typedef struct vulkan_context {
    /**
     * @brief The VkInstance object that represents the connection between the application and the Vulkan library.
     */
    VkInstance instance;

    /**
     * @brief Optional custom allocator for Vulkan memory allocations.
     * If NULL, the default allocator is used.
     */
    VkAllocationCallbacks* allocator;
} vulkan_context;