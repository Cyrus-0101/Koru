#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_swapchain.h
 * @brief Vulkan swapchain management interface for the Koru Engine.
 *
 * This module provides functions to:
 * - Create and destroy a Vulkan swapchain
 * - Recreate it when the window is resized
 * - Acquire image indices for rendering
 * - Present rendered images to the screen
 *
 * The swapchain acts as a queue of framebuffers that can be rendered to and presented,
 * enabling smooth animation and synchronization between CPU/GPU operations.
 */

/**
 * @brief Creates a new Vulkan swapchain.
 *
 * Initializes the swapchain based on surface capabilities, format, and present mode.
 * Must be called after logical device creation and before rendering begins.
 *
 * @param context A pointer to the active Vulkan context.
 * @param width The desired width of the swapchain images.
 * @param height The desired height of the swapchain images.
 * @param out_swapchain A pointer to a vulkan_swapchain structure to populate.
 */
void vulkan_swapchain_create(
    vulkan_context* context,
    u32 width,
    u32 height,
    vulkan_swapchain* out_swapchain);

/**
 * @brief Recreates an existing swapchain, typically after a window resize event.
 *
 * Destroys the old swapchain and creates a new one with updated dimensions.
 * Used during window resize handling to maintain valid render targets.
 *
 * @param context A pointer to the active Vulkan context.
 * @param width New width of the swapchain images.
 * @param height New height of the swapchain images.
 * @param swapchain A pointer to the existing swapchain to recreate.
 */
void vulkan_swapchain_recreate(
    vulkan_context* context,
    u32 width,
    u32 height,
    vulkan_swapchain* swapchain);

/**
 * @brief Destroys a Vulkan swapchain and releases all associated resources.
 *
 * Cleans up:
 * - Swapchain handle
 * - Image views
 * - Framebuffers
 * - Render passes
 *
 * Should be called before destroying the logical device.
 *
 * @param context A pointer to the active Vulkan context.
 * @param swapchain A pointer to the swapchain to destroy.
 */
void vulkan_swapchain_destroy(
    vulkan_context* context,
    vulkan_swapchain* swapchain);

/**
 * @brief Acquires the next available image index from the swapchain.
 *
 * This function waits until a swapchain image is ready to be rendered to.
 * Returns an index that can be used to select the appropriate image from the swapchain.
 *
 * @param context A pointer to the active Vulkan context.
 * @param swapchain A pointer to the swapchain to query.
 * @param timeout_ns Maximum time to wait for an image before timing out.
 * @param image_available_semaphore Semaphore signaled when the image is ready for use.
 * @param fence Optional fence to wait on before using the image.
 * @param out_image_index Output variable to store the index of the acquired image.
 * @return True if successful; False otherwise.
 */
b8 vulkan_swapchain_acquire_next_image_index(
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    u64 timeout_ns,
    VkSemaphore image_available_semaphore,
    VkFence fence,
    u32* out_image_index);

/**
 * @brief Presents the rendered image at the given index to the screen.
 *
 * Submits the final rendered image to the presentation queue.
 * Must be called after command buffer submission completes.
 *
 * @param context A pointer to the active Vulkan context.
 * @param swapchain A pointer to the swapchain being used.
 * @param graphics_queue Graphics queue handle used for rendering.
 * @param present_queue Presentation queue handle used for display.
 * @param render_complete_semaphore Semaphore signaled when rendering is complete.
 * @param present_image_index Index of the image to present.
 */
void vulkan_swapchain_present(
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_semaphore,
    u32 present_image_index);