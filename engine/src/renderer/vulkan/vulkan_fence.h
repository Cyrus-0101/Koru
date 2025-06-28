#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_fence.h
 * @brief Fence management utilities for the Koru Engine's Vulkan renderer.
 *
 * This module provides functions to:
 * - Create and destroy VkFence objects
 * - Wait for fences (CPU waits for GPU)
 * - Reset fences for reuse
 *
 * Fences are used during rendering to ensure safe access to resources like:
 * - Command buffers
 * - Swapchain images
 * - Memory allocations
 */

/**
 * @brief Creates a new Vulkan fence.
 *
 * Fences are used to synchronize the CPU with GPU operations.
 * A fence starts in either:
 * - Unsignaled state (default)
 * - Signaled state (`create_signaled == True`)
 *
 * @param context A pointer to the active Vulkan context.
 * @param create_signaled If True, the fence will be created in a signaled state.
 * @param out_fence A pointer to the vulkan_fence struct to populate.
 */
void vulkan_fence_create(
    vulkan_context* context,
    b8 create_signaled,
    vulkan_fence* out_fence);

/**
 * @brief Destroys a Vulkan fence and releases its resources.
 *
 * Must be called before destroying the logical device.
 *
 * @param context A pointer to the active Vulkan context.
 * @param fence A pointer to the vulkan_fence to destroy.
 */
void vulkan_fence_destroy(vulkan_context* context, vulkan_fence* fence);

/**
 * @brief Waits for the given fence to be signaled by the GPU.
 *
 * If the fence is already signaled, returns immediately.
 * Otherwise, waits up to `timeout_ns` nanoseconds for it to become signaled.
 *
 * @param context A pointer to the active Vulkan context.
 * @param fence A pointer to the vulkan_fence to wait for.
 * @param timeout_ns Maximum time to wait in nanoseconds.
 * @return True if the fence was signaled within the timeout; False otherwise.
 */
b8 vulkan_fence_wait(vulkan_context* context, vulkan_fence* fence, u64 timeout_ns);

/**
 * @brief Resets the fence so it can be reused.
 *
 * Only resets the fence if it was previously signaled.
 *
 * @param context A pointer to the active Vulkan context.
 * @param fence A pointer to the vulkan_fence to reset.
 */
void vulkan_fence_reset(vulkan_context* context, vulkan_fence* fence);