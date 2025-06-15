#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_framebuffer.h
 * @brief Framebuffer management interface for the Koru Engine's Vulkan renderer.
 *
 * This module provides functions to:
 * - Create framebuffers that bind image views to a render pass
 * - Destroy them safely during shutdown
 *
 * Framebuffers are tightly coupled to the swapchain and must be recreated when:
 * - The window resizes
 * - The swapchain is re-created (e.g., due to vsync toggle or resolution change)
 */

/**
 * @brief Creates a new Vulkan framebuffer using the provided attachments and render pass.
 *
 * Wraps `vkCreateFramebuffer(...)` and stores the attachments internally.
 * Used when creating or recreating framebuffers after swapchain changes.
 *
 * @param context A pointer to the active Vulkan context.
 * @param renderpass A pointer to the render pass this framebuffer will use.
 * @param width Width of the framebuffer in pixels.
 * @param height Height of the framebuffer in pixels.
 * @param attachment_count Number of image views being bound.
 * @param attachments An array of VkImageView objects to attach.
 * @param out_framebuffer A pointer to a vulkan_framebuffer structure to populate.
 */
void vulkan_framebuffer_create(
    vulkan_context* context,
    vulkan_renderpass* renderpass,
    u32 width,
    u32 height,
    u32 attachment_count,
    VkImageView* attachments,
    vulkan_framebuffer* out_framebuffer);

/**
 * @brief Destroys a Vulkan framebuffer and releases its resources.
 *
 * Also frees any internal attachment memory if needed.
 * Must be called before destroying the logical device or render pass.
 *
 * @param context A pointer to the active Vulkan context.
 * @param framebuffer A pointer to the vulkan_framebuffer to destroy.
 */
void vulkan_framebuffer_destroy(vulkan_context* context, vulkan_framebuffer* framebuffer);