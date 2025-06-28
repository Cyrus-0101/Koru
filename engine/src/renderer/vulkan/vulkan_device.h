#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_device.h
 * @brief Vulkan device management interface.
 *
 * This module provides functions to:
 * - Query available physical devices
 * - Select and create a logical device
 * - Query swapchain support on a given device and surface
 *
 * These functions abstract away platform-specific device selection logic,
 * making it easier to integrate with higher-level rendering systems.
 */

/**
 * @brief Creates and selects a suitable physical/logical Vulkan device.
 *
 * Queries all available physical devices, selects one based on feature and queue support,
 * creates a logical device, and initializes queues (graphics, present, compute, transfer).
 *
 * @param context A pointer to the Vulkan context. Must be valid and initialized.
 * @return True if a suitable device was found and successfully created; False otherwise.
 */
b8 vulkan_device_create(vulkan_context* context);

/**
 * @brief Destroys the logical device and cleans up associated resources.
 *
 * Shuts down the logical device and any internal structures like queues.
 * Should be called before shutting down the instance.
 *
 * @param context A pointer to the Vulkan context containing the device to destroy.
 */
void vulkan_device_destroy(vulkan_context* context);

/**
 * @brief Queries swapchain support details for a given physical device and surface.
 *
 * Populates a `vulkan_swapchain_support_info` struct with:
 * - Capabilities (min/max image counts, dimensions, etc.)
 * - Formats supported
 * - Present modes available
 *
 * This information is used later when creating a swapchain.
 *
 * @param physical_device The physical device to query.
 * @param surface The surface to check compatibility with.
 * @param out_support_info A pointer to a structure to fill with swapchain support info.
 */
void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info* out_support_info);

/**
 * @brief Detects and selects a suitable depth/stencil format for the given device.
 *
 * Tries common depth formats (D32_FLOAT, D24_UNORM_S8_UINT, etc.)
 * and checks if they are supported with optimal tiling.
 *
 * @param device A pointer to the vulkan_device object.
 * @return True if a valid depth format was found; False otherwise.
 */
b8 vulkan_device_detect_depth_format(vulkan_device* device);