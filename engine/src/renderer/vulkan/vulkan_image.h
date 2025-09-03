#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_image.h
 * @brief Image management utilities for the Koru Engine's Vulkan renderer.
 *
 * This module provides functions to:
 * - Create and allocate Vulkan images
 * - Generate image views for use in render passes or shaders
 * - Destroy images and associated resources
 *
 * These are used throughout the rendering pipeline for color attachments,
 * depth/stencil buffers, and texture resources.
 */

/**
 * @brief Creates and allocates a Vulkan image with specified properties.
 *
 * Handles:
 * - Image creation (`vkCreateImage`)
 * - Memory allocation and binding
 * - Optional image view creation
 *
 * Usage example: creating swapchain images, depth attachments, or textures.
 *
 * @param context A pointer to the active Vulkan context.
 * @param image_type Type of image (1D, 2D, 3D).
 * @param width Width of the image in pixels.
 * @param height Height of the image in pixels.
 * @param format Pixel format (e.g., VK_FORMAT_B8G8R8A8_UNORM).
 * @param tiling Tiling mode (linear or optimal).
 * @param usage Usage flags indicating how this image will be used.
 * @param memory_flags Flags for memory allocation (device/local, host visible, etc.)
 * @param create_view Whether to automatically generate an image view.
 * @param view_aspect_flags Aspect flags for image view creation (color, depth, etc.).
 * @param out_image A pointer to a vulkan_image struct to populate.
 */
void vulkan_image_create(
    vulkan_context* context,
    VkImageType image_type,
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memory_flags,
    b32 create_view,
    VkImageAspectFlags view_aspect_flags,
    vulkan_image* out_image);

/**
 * @brief Creates an image view for a given image.
 *
 * Image views are necessary when using an image as:
 * - Render target (color/depth attachment)
 * - Texture (sampled image)
 * - Storage image (for compute shaders)
 *
 * @param context A pointer to the active Vulkan context.
 * @param format Format of the image data.
 * @param image A pointer to the vulkan_image object to create a view for.
 * @param aspect_flags Describes which part of the image is accessible through the view (color, depth, etc.).
 */
void vulkan_image_view_create(
    vulkan_context* context,
    VkFormat format,
    vulkan_image* image,
    VkImageAspectFlags aspect_flags);

/**
 * @brief Destroys a Vulkan image and its associated resources.
 *
 * Cleans up:
 * - Image handle (`vkDestroyImage`)
 * - Allocated memory (`vkFreeMemory`)
 * - Image view (if present)
 *
 * Must be called before destroying the logical device.
 *
 * @param context A pointer to the active Vulkan context.
 * @param image A pointer to the vulkan_image to destroy.
 */
void vulkan_image_destroy(vulkan_context* context, vulkan_image* image);

/**
 * @brief Transitions a Vulkan image from one layout to another.
 *
 * Image layout transitions are required when changing how an image is used,
 * such as transitioning from undefined to a color attachment layout before rendering,
 * or from a color attachment to shader read-only for sampling in a shader.
 *
 * This function records the necessary pipeline barrier into the provided command buffer.
 *
 * @param context A pointer to the active Vulkan context.
 * @param command_buffer A pointer to the command buffer to record the transition into.
 * @param image A pointer to the vulkan_image to transition.
 * @param format Format of the image (used for depth/stencil aspects).
 * @param old_layout The current layout of the image.
 * @param new_layout The desired layout of the image.
 */
void vulkan_image_transition_layout(
    vulkan_context* context,
    vulkan_command_buffer* command_buffer,
    vulkan_image* image,
    VkFormat format,
    VkImageLayout old_layout,
    VkImageLayout new_layout);

/**
 * @brief Copies data from a buffer to a Vulkan image.
 *
 * Used for uploading texture data or other image content from CPU-visible buffers.
 *
 * @param context A pointer to the active Vulkan context.
 * @param image A pointer to the destination vulkan_image.
 * @param buffer The source VkBuffer containing the data to copy.
 * @param command_buffer A command buffer to record the copy operation into.
 */
void vulkan_image_copy_from_buffer(
    vulkan_context* context,
    vulkan_image* image,
    VkBuffer buffer,
    vulkan_command_buffer* command_buffer);

/**
 * @brief Destroys a Vulkan image and its associated resources.
 *
 * @param context A pointer to the active Vulkan context.
 * @param image A pointer to the vulkan_image to destroy.
 */
void vulkan_image_destroy(vulkan_context* context, vulkan_image* image);