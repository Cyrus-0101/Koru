#include "core/kmemory.h"
#include "vulkan_renderpass.h"

#define ATTACHMENT_DESCRIPTION_COUNT 2

/**
 * @file vulkan_renderpass.c
 * @brief Implementation of Vulkan render pass creation, begin, and end functions.
 *
 * This module provides utilities to:
 * - Create a render pass with color and depth attachments
 * - Begin rendering into a framebuffer
 * - End the render pass after drawing is complete
 *
 * It works closely with:
 * - `vulkan_swapchain`
 * - `vulkan_framebuffer`
 * - `vulkan_command_buffer`
 */

/**
 * @brief Creates a new render pass using the provided configuration.
 *
 * Sets up:
 * - One subpass (currently hard-coded)
 * - Two attachment descriptions: one for color, one for depth/stencil
 * - LoadOp/Clear values
 * - Image layout transitions
 *
 * @param context A pointer to the active Vulkan context.
 * @param out_renderpass A pointer to the vulkan_renderpass struct to populate.
 * @param x X position of the viewport.
 * @param y Y position of the viewport.
 * @param w Width of the viewport.
 * @param h Height of the viewport.
 * @param r Red clear value (0–1).
 * @param g Green clear value (0–1).
 * @param b Blue clear value (0–1).
 * @param a Alpha clear value (0–1).
 * @param depth Depth clear value (0–1).
 * @param stencil Stencil clear value.
 */
void vulkan_renderpass_create(
    vulkan_context* context,
    vulkan_renderpass* out_renderpass,
    f32 x, f32 y, f32 w, f32 h,
    f32 r, f32 g, f32 b, f32 a,
    f32 depth,
    u32 stencil) {

    // Copy viewport and clear values from input
    out_renderpass->x = x;
    out_renderpass->y = y;
    out_renderpass->w = w;
    out_renderpass->h = h;

    out_renderpass->r = r;
    out_renderpass->g = g;
    out_renderpass->b = b;
    out_renderpass->a = a;

    out_renderpass->depth = depth;
    out_renderpass->stencil = stencil;

    // Main Subpass setup
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachment Descriptions. TODO: make this configurable.
    VkAttachmentDescription attachment_descriptions[ATTACHMENT_DESCRIPTION_COUNT];

    // Color attachment description
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = context->swapchain.image_format.format; // TODO: Make configurable
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Do not expect any particular layout before render pass starts.
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Transitioned to after the render pass
    color_attachment.flags = 0;

    attachment_descriptions[0] = color_attachment;

    // Depth attachment description
    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = context->device.depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_attachment.flags = 0;

    attachment_descriptions[1] = depth_attachment;

    // Color attachment reference
    VkAttachmentReference color_attachment_reference = {};
    color_attachment_reference.attachment = 0;  // Index into pAttachments array
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    // Depth/stencil attachment reference
    VkAttachmentReference depth_attachment_reference = {};
    depth_attachment_reference.attachment = 1;
    depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    subpass.pDepthStencilAttachment = &depth_attachment_reference;

    // Input attachment count (not used yet)
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = 0;

    // Resolve attachments for multisampling (not used yet)
    subpass.pResolveAttachments = 0;

    // Preserve attachments between subpasses (not used yet)
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = 0;

    // Render pass dependency (transition from outside to first subpass)
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    // Build render pass create info
    VkRenderPassCreateInfo render_pass_create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_pass_create_info.attachmentCount = ATTACHMENT_DESCRIPTION_COUNT;
    render_pass_create_info.pAttachments = attachment_descriptions;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &dependency;
    render_pass_create_info.pNext = 0;
    render_pass_create_info.flags = 0;

    // Create the render pass
    VK_CHECK(vkCreateRenderPass(context->device.logical_device, &render_pass_create_info, context->allocator, &out_renderpass->handle));
}

/**
 * @brief Destroys a render pass and cleans up its resources.
 *
 * Must be called before destroying the logical device.
 *
 * @param context A pointer to the active Vulkan context.
 * @param renderpass A pointer to the vulkan_renderpass being destroyed.
 */
void vulkan_renderpass_destroy(vulkan_context* context, vulkan_renderpass* renderpass) {
    if (renderpass && renderpass->handle) {
        vkDestroyRenderPass(context->device.logical_device, renderpass->handle, context->allocator);
        renderpass->handle = 0;
    }
}

/**
 * @brief Begins a render pass using the given command buffer and framebuffer.
 *
 * Records the beginning of the render pass into the command buffer.
 * Sets up:
 * - Render area (viewport)
 * - Clear values (color, depth, stencil)
 * - Framebuffer binding
 *
 * After this call, the command buffer is in `IN_RENDER_PASS` state.
 *
 * @param command_buffer A pointer to the active command buffer.
 * @param renderpass A pointer to the vulkan_renderpass to begin.
 * @param frame_buffer The framebuffer to render into.
 */
void vulkan_renderpass_begin(
    vulkan_command_buffer* command_buffer,
    vulkan_renderpass* renderpass,
    VkFramebuffer frame_buffer) {

    // Setup render area
    VkRenderPassBeginInfo begin_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    begin_info.renderPass = renderpass->handle;
    begin_info.framebuffer = frame_buffer;
    begin_info.renderArea.offset.x = renderpass->x;
    begin_info.renderArea.offset.y = renderpass->y;
    begin_info.renderArea.extent.width = renderpass->w;
    begin_info.renderArea.extent.height = renderpass->h;

    // Setup clear values
    VkClearValue clear_values[2];
    kzero_memory(clear_values, sizeof(VkClearValue) * 2);

    clear_values[0].color.float32[0] = renderpass->r;
    clear_values[0].color.float32[1] = renderpass->g;
    clear_values[0].color.float32[2] = renderpass->b;
    clear_values[0].color.float32[3] = renderpass->a;

    clear_values[1].depthStencil.depth = renderpass->depth;
    clear_values[1].depthStencil.stencil = renderpass->stencil;

    begin_info.clearValueCount = 2;
    begin_info.pClearValues = clear_values;

    // Start recording commands inside the render pass
    vkCmdBeginRenderPass(command_buffer->handle, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

    // Update command buffer state
    command_buffer->state = COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

/**
 * @brief Ends the current render pass.
 *
 * Records the end of the render pass into the command buffer.
 * Updates the command buffer state back to RECORDING.
 *
 * @param command_buffer A pointer to the active command buffer.
 * @param renderpass A pointer to the vulkan_renderpass being ended.
 */
void vulkan_renderpass_end(vulkan_command_buffer* command_buffer, vulkan_renderpass* renderpass) {
    // End the render pass
    vkCmdEndRenderPass(command_buffer->handle);

    // Update command buffer state
    command_buffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}