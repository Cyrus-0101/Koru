#include "core/kmemory.h"
#include "core/logger.h"
#include "vulkan_device.h"
#include "vulkan_image.h"

/**
 * @file vulkan_image.c
 * @brief Implementation of Vulkan image management utilities.
 *
 * This module provides functions to:
 * - Create and allocate VkImage objects
 * - Generate VkImageViews for use in render passes or shaders
 * - Destroy images and release associated resources
 *
 * These utilities are used throughout the engine for:
 * - Swapchain image setup
 * - Depth/stencil buffers
 * - Texture loading and management
 */

/**
 * @brief Creates and allocates a new Vulkan image with the specified properties.
 *
 * Handles:
 * - Image creation (vkCreateImage)
 * - Memory allocation and binding
 * - Optional image view creation (via vulkan_image_view_create)
 *
 * @param context A pointer to the active Vulkan context.
 * @param image_type Type of image (1D, 2D, 3D).
 * @param width Width of the image in pixels.
 * @param height Height of the image in pixels.
 * @param format Pixel format (e.g., VK_FORMAT_B8G8R8A8_UNORM).
 * @param tiling Tiling mode: VK_IMAGE_TILING_LINEAR or VK_IMAGE_TILING_OPTIMAL.
 * @param usage Usage flags indicating how this image will be used.
 * @param memory_flags Flags for memory allocation (device/local, host visible, etc.).
 * @param create_view Whether to automatically generate an image view.
 * @param view_aspect_flags Aspect flags for image view (color, depth, etc.).
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
    vulkan_image* out_image) {
    // Copy params to output structure
    out_image->width = width;
    out_image->height = height;

    // Set up image creation info
    VkImageCreateInfo image_create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.extent.depth = 1;  // TODO: Support configurable depth (e.g., 3D textures or volume data)
    image_create_info.mipLevels = 4;     // TODO: Add support for mip mapping and make this configurable
    image_create_info.arrayLayers = 1;   // TODO: Support array layers for texture arrays
    image_create_info.format = format;
    image_create_info.tiling = tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;          // TODO: Configurable MSAA sample count
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // TODO: Make sharing mode configurable

    // Create the image
    VK_CHECK(vkCreateImage(context->device.logical_device, &image_create_info, context->allocator, &out_image->handle));

    // Query memory requirements for the created image
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device.logical_device, out_image->handle, &memory_requirements);

    // Find suitable memory type
    i32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    if (memory_type == -1) {
        KERROR("Required memory type not found. Image not valid.");
    }

    // Allocate device memory for the image
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VK_CHECK(vkAllocateMemory(context->device.logical_device, &memory_allocate_info, context->allocator, &out_image->memory));

    // Bind allocated memory to the image handle
    VK_CHECK(vkBindImageMemory(context->device.logical_device, out_image->handle, out_image->memory, 0));  // TODO: Make offset configurable

    // Optionally create an image view
    if (create_view) {
        out_image->view = 0;
        vulkan_image_view_create(context, format, out_image, view_aspect_flags);
    }
}

/**
 * @brief Creates an image view for the given image.
 *
 * Image views define how the image is accessed (e.g., as a color attachment,
 * sampled image, depth buffer, etc.)
 *
 * @param context A pointer to the active Vulkan context.
 * @param format Format of the image data.
 * @param image A pointer to the vulkan_image object to create a view for.
 * @param aspect_flags Describes which part of the image is accessible through the view.
 */
void vulkan_image_view_create(
    vulkan_context* context,
    VkFormat format,
    vulkan_image* image,
    VkImageAspectFlags aspect_flags) {
    // Set up image view creation info
    VkImageViewCreateInfo view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_create_info.image = image->handle;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;  // TODO: Allow configuration for 1D/3D or cube maps
    view_create_info.format = format;
    view_create_info.subresourceRange.aspectMask = aspect_flags;

    // Subresource range defaults
    view_create_info.subresourceRange.baseMipLevel = 0;  // TODO: Configurable mipmap level
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = 1;

    // Create the image view
    VK_CHECK(vkCreateImageView(context->device.logical_device, &view_create_info, context->allocator, &image->view));
}

void vulkan_image_transition_layout(
    vulkan_context* context,
    vulkan_command_buffer* command_buffer,
    vulkan_image* image,
    VkFormat format,
    VkImageLayout old_layout,
    VkImageLayout new_layout) {
    VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};

    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;

    barrier.srcQueueFamilyIndex = context->device.graphics_queue_index;
    barrier.dstQueueFamilyIndex = context->device.graphics_queue_index;

    barrier.image = image->handle;

    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags dest_stage;

    // Don't care about the old layout - transition to optimal layout (for the underlying implementation).
    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;

        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        // Don't care what stage the pipeline is in at the start.
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        // Used for copying
        dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Transitioning from a transfer destination layout to a shader-readonly layout.
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        // From a copying stage to...
        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        // The fragment stage.
        dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        KFATAL("unsupported layout transition!");

        return;
    }

    vkCmdPipelineBarrier(
        command_buffer->handle,
        source_stage, dest_stage,
        0,
        0, 0,
        0, 0,
        1, &barrier);
}

void vulkan_image_copy_from_buffer(
    vulkan_context* context,
    vulkan_image* image,
    VkBuffer buffer,
    vulkan_command_buffer* command_buffer) {
    // Region to copy
    VkBufferImageCopy region;

    kzero_memory(&region, sizeof(VkBufferImageCopy));

    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageExtent.width = image->width;
    region.imageExtent.height = image->height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(
        command_buffer->handle,
        buffer,
        image->handle,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}

/**
 * @brief Destroys a Vulkan image and its associated resources.
 *
 * Cleans up:
 * - The image view (if present)
 * - Allocated memory for the image
 * - The image handle itself
 *
 * Must be called before destroying the logical device.
 *
 * @param context A pointer to the active Vulkan context.
 * @param image A pointer to the vulkan_image to destroy.
 */
void vulkan_image_destroy(vulkan_context* context, vulkan_image* image) {
    // Destroy image view first
    if (image->view) {
        vkDestroyImageView(context->device.logical_device, image->view, context->allocator);
        image->view = 0;
    }

    // Free image memory
    if (image->memory) {
        vkFreeMemory(context->device.logical_device, image->memory, context->allocator);
        image->memory = 0;
    }

    // Destroy image handle last
    if (image->handle) {
        vkDestroyImage(context->device.logical_device, image->handle, context->allocator);
        image->handle = 0;
    }
}