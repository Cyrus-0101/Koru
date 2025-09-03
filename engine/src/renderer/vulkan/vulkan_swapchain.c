#include "core/kmemory.h"
#include "core/logger.h"
#include "vulkan_device.h"
#include "vulkan_image.h"
#include "vulkan_swapchain.h"

/**
 * @file vulkan_swapchain.c
 * @brief Implementation of Vulkan swapchain creation, recreation, and destruction.
 *
 * This module handles:
 * - Swapchain creation using device and surface capabilities
 * - Recreation when window resizes or becomes outdated
 * - Proper cleanup of images and image views
 * - Integration with render pass and depth buffer systems
 *
 * It works closely with `vulkan_device`, `vulkan_image`, and `vulkan_renderpass` modules.
 */

///> Forward Declarations

/**
 * @brief Internal function to create a new Vulkan swapchain.
 *
 * This helper creates the actual VkSwapchainKHR object based on surface capabilities,
 * selected format, and present mode. Called during initial creation or recreation.
 *
 * @param context A pointer to the active Vulkan context.
 * @param width The desired width of the swapchain images.
 * @param height The desired height of the swapchain images.
 * @param swapchain A pointer to the vulkan_swapchain structure to populate.
 */
void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain);

/**
 * @brief Internal function to destroy a Vulkan swapchain.
 *
 * Cleans up image views and the swapchain handle itself.
 * Called during swapchain destruction or recreation.
 *
 * @param context A pointer to the active Vulkan context.
 * @param swapchain A pointer to the vulkan_swapchain to destroy.
 */
void destroy(vulkan_context* context, vulkan_swapchain* swapchain);

/**
 * @brief Creates and initializes a Vulkan swapchain for rendering.
 *
 * Sets up:
 * - Surface format selection (preferred B8G8R8A8_UNORM)
 * - Present mode (prefers MAILBOX_KHR if available)
 * - Image count and extent clamped to supported values
 * - Image views for each swapchain image
 * - Depth attachment for rendering
 *
 * @param context A pointer to the active Vulkan context.
 * @param width The width of the new swapchain.
 * @param height The height of the new swapchain.
 * @param out_swapchain A pointer to the vulkan_swapchain structure to populate.
 */
void vulkan_swapchain_create(
    vulkan_context* context,
    u32 width,
    u32 height,
    vulkan_swapchain* out_swapchain) {
    // Simply create a new one.
    create(context, width, height, out_swapchain);
}

/**
 * @brief Recreates the swapchain after resize or resolution change.
 *
 * Destroys the current swapchain and recreates it with updated dimensions.
 *
 * @param context A pointer to the active Vulkan context.
 * @param width New width of the swapchain.
 * @param height New height of the swapchain.
 * @param swapchain A pointer to the existing vulkan_swapchain to update.
 */
void vulkan_swapchain_recreate(
    vulkan_context* context,
    u32 width,
    u32 height,
    vulkan_swapchain* swapchain) {
    // Destroy the old and create a new one.
    destroy(context, swapchain);
    create(context, width, height, swapchain);
}

/**
 * @brief Destroys the given swapchain and associated resources.
 *
 * Frees all image views and destroys the internal VkSwapchainKHR object.
 *
 * @param context A pointer to the active Vulkan context.
 * @param swapchain A pointer to the vulkan_swapchain to destroy.
 */
void vulkan_swapchain_destroy(
    vulkan_context* context,
    vulkan_swapchain* swapchain) {
    destroy(context, swapchain);
}

/**
 * @brief Acquires the index of the next available image from the swapchain.
 *
 * Waits for an image to become available, then returns its index.
 * If the swapchain is outdated, triggers recreation logic.
 *
 * @param context A pointer to the active Vulkan context.
 * @param swapchain A pointer to the vulkan_swapchain being used.
 * @param timeout_ns Maximum time to wait for an image.
 * @param image_available_semaphore Semaphore signaled when image is ready.
 * @param fence Optional fence to wait on before using the image.
 * @param out_image_index Output variable to store the acquired image index.
 * @return True if successful; False if the swapchain needs recreation.
 */
b8 vulkan_swapchain_acquire_next_image_index(
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    u64 timeout_ns,
    VkSemaphore image_available_semaphore,
    VkFence fence,
    u32* out_image_index) {
    VkResult result = vkAcquireNextImageKHR(
        context->device.logical_device,
        swapchain->handle,
        timeout_ns,
        image_available_semaphore,
        fence,
        out_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // Trigger swapchain recreation, then boot out of the render loop.
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
        return False;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        KFATAL("Failed to acquire swapchain image!");
        return False;
    }

    return True;
}

/**
 * @brief Presents the rendered image at the given index to the screen.
 *
 * Submits the final frame to the presentation queue using the provided semaphore.
 * Handles errors like out-of-date swapchains by triggering recreation.
 *
 * @param context A pointer to the active Vulkan context.
 * @param swapchain A pointer to the vulkan_swapchain being used.
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
    u32 present_image_index) {
    // Return the image to the swapchain for presentation.
    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain->handle;
    present_info.pImageIndices = &present_image_index;
    present_info.pResults = 0;

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
    } else if (result != VK_SUCCESS) {
        KFATAL("Failed to present swapchain image!");
    }

    // Increment (and loop) the index.
    context->current_frame = (context->current_frame + 1) % swapchain->max_frames_in_flight;
}

/**
 * @brief Internal implementation of creating a new swapchain.
 *
 * Performs the actual creation logic including:
 * - Choosing the best image format and present mode
 * - Setting up image sharing between queues
 * - Creating image views
 * - Initializing depth/stencil buffer
 *
 * @param context A pointer to the active Vulkan context.
 * @param width The width of the new swapchain.
 * @param height The height of the new swapchain.
 * @param swapchain A pointer to the vulkan_swapchain structure to populate.
 */
void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain) {
    VkExtent2D swapchain_extent = {width, height};

    // Choose a swap surface format.
    b8 found = False;
    for (u32 i = 0; i < context->device.swapchain_support.format_count; ++i) {
        VkSurfaceFormatKHR format = context->device.swapchain_support.formats[i];
        // Preferred formats: B8G8R8A8_UNORM + sRGB color space
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapchain->image_format = format;
            found = True;
            break;
        }
    }

    if (!found) {
        swapchain->image_format = context->device.swapchain_support.formats[0];
    }

    // Choose a present mode
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;  // Default: vsync
    for (u32 i = 0; i < context->device.swapchain_support.present_mode_count; ++i) {
        VkPresentModeKHR mode = context->device.swapchain_support.present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = mode;
            break;
        }
    }

    // Requery swapchain support
    vulkan_device_query_swapchain_support(
        context->device.physical_device,
        context->surface,
        &context->device.swapchain_support);

    // Set the correct extent
    if (context->device.swapchain_support.capabilities.currentExtent.width != UINT32_MAX) {
        swapchain_extent = context->device.swapchain_support.capabilities.currentExtent;
    }

    // Clamp to min/max allowed by the device
    VkExtent2D min = context->device.swapchain_support.capabilities.minImageExtent;
    VkExtent2D max = context->device.swapchain_support.capabilities.maxImageExtent;
    swapchain_extent.width = KCLAMP(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = KCLAMP(swapchain_extent.height, min.height, max.height);

    // Determine image count (one more than minimum, but not exceeding maximum)
    u32 image_count = context->device.swapchain_support.capabilities.minImageCount + 1;
    if (context->device.swapchain_support.capabilities.maxImageCount > 0 && image_count > context->device.swapchain_support.capabilities.maxImageCount) {
        image_count = context->device.swapchain_support.capabilities.maxImageCount;
    }

    swapchain->max_frames_in_flight = image_count;

    // Build the swapchain creation info
    VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_create_info.surface = context->surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = swapchain->image_format.format;
    swapchain_create_info.imageColorSpace = swapchain->image_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Setup image sharing mode & the queue family indices
    if (context->device.graphics_queue_index != context->device.present_queue_index) {
        u32 queueFamilyIndices[] = {
            (u32)context->device.graphics_queue_index,
            (u32)context->device.present_queue_index};
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = 0;
    }

    // Transform settings
    swapchain_create_info.preTransform = context->device.swapchain_support.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = 0;

    // Create the swapchain
    VK_CHECK(vkCreateSwapchainKHR(context->device.logical_device, &swapchain_create_info, context->allocator, &swapchain->handle));

    // Reset frame index
    context->current_frame = 0;

    // Get images
    swapchain->image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, swapchain->handle, &swapchain->image_count, 0));
    if (!swapchain->images) {
        swapchain->images = (VkImage*)kallocate(sizeof(VkImage) * swapchain->image_count, MEMORY_TAG_RENDERER);
    }
    if (!swapchain->views) {
        swapchain->views = (VkImageView*)kallocate(sizeof(VkImageView) * swapchain->image_count, MEMORY_TAG_RENDERER);
    }

    // Retrieve the actual images from the swapchain
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device, swapchain->handle, &swapchain->image_count, swapchain->images));

    // Create image views for each image
    for (u32 i = 0; i < swapchain->image_count; ++i) {
        VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view_info.image = swapchain->images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swapchain->image_format.format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(context->device.logical_device, &view_info, context->allocator, &swapchain->views[i]));
    }

    // Detect and set depth format
    if (!vulkan_device_detect_depth_format(&context->device)) {
        context->device.depth_format = VK_FORMAT_UNDEFINED;
        KFATAL("Failed to find a supported depth format!");
    }

    // Create depth image and view
    vulkan_image_create(
        context,
        VK_IMAGE_TYPE_2D,
        swapchain_extent.width,
        swapchain_extent.height,
        context->device.depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        True,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &swapchain->depth_attachment);

    KINFO("Swapchain created successfully.");
}

/**
 * @brief Internal implementation of destroying a swapchain.
 *
 * Destroys the depth attachment, image views, and the swapchain handle.
 *
 * @param context A pointer to the active Vulkan context.
 * @param swapchain A pointer to the vulkan_swapchain to destroy.
 */
void destroy(vulkan_context* context, vulkan_swapchain* swapchain) {
    vkDeviceWaitIdle(context->device.logical_device);

    vulkan_image_destroy(context, &swapchain->depth_attachment);

    // Destroy image views (images are owned by the swapchain and will be destroyed automatically)
    for (u32 i = 0; i < swapchain->image_count; ++i) {
        vkDestroyImageView(context->device.logical_device, swapchain->views[i], context->allocator);
    }

    // Destroy the swapchain itself
    vkDestroySwapchainKHR(context->device.logical_device, swapchain->handle, context->allocator);
}