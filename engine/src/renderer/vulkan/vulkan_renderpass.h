#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_renderpass.h
 * @brief Vulkan render pass management interface for the Koru Engine.
 *
 * This module provides functions to:
 * - Create and destroy render passes
 * - Begin and end rendering operations within a render pass
 *
 * Render passes define how rendering commands interact with attachments like:
 * - Color buffers (swapchain images)
 * - Depth/stencil buffers
 * - Subpasses and load/store operations
 */

/**
 * @brief Creates a new render pass using the provided configuration.
 *
 * Sets up:
 * - One subpass
 * - Two attachment references: one for color, one for depth/stencil
 * - LoadOp: CLEAR, StoreOp: STORE
 * - Image layout transitions
 *
 * @param context A pointer to the active Vulkan context.
 * @param out_renderpass A pointer to the vulkan_renderpass struct to populate.
 * @param x X position of the viewport.
 * @param y Y position of the viewport.
 * @param w Width of the viewport.
 * @param h Height of the viewport.
 * @param r Red clear value.
 * @param g Green clear value.
 * @param b Blue clear value.
 * @param a Alpha clear value.
 * @param depth Depth clear value.
 * @param stencil Stencil clear value.
 */
void vulkan_renderpass_create(
    vulkan_context* context, 
    vulkan_renderpass* out_renderpass,
    f32 x, f32 y, f32 w, f32 h,
    f32 r, f32 g, f32 b, f32 a,
    f32 depth,
    u32 stencil);

/**
 * @brief Destroys a render pass and cleans up its resources.
 *
 * Must be called before destroying the logical device.
 *
 * @param context A pointer to the active Vulkan context.
 * @param renderpass A pointer to the vulkan_renderpass to destroy.
 */
void vulkan_renderpass_destroy(vulkan_context* context, vulkan_renderpass* renderpass);

/**
 * @brief Begins a render pass using the given command buffer.
 *
 * Records the beginning of the render pass, sets up:
 * - Framebuffer
 * - Render area
 * - Clear values
 * - Subpass index = 0
 *
 * @param command_buffer A pointer to the active command buffer.
 * @param renderpass A pointer to the vulkan_renderpass to begin.
 * @param frame_buffer The framebuffer to render into.
 */
void vulkan_renderpass_begin(
    vulkan_command_buffer* command_buffer, 
    vulkan_renderpass* renderpass,
    VkFramebuffer frame_buffer);

/**
 * @brief Ends the current render pass.
 *
 * Records the end of the render pass into the command buffer.
 *
 * @param command_buffer A pointer to the active command buffer.
 * @param renderpass A pointer to the vulkan_renderpass being ended.
 */
void vulkan_renderpass_end(
    vulkan_command_buffer* command_buffer, 
    vulkan_renderpass* renderpass);