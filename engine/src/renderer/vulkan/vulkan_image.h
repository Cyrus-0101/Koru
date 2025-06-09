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