#include "core/logger.h"
#include "vulkan_backend.h"
#include "vulkan_types.inl"

/**
 * @file vulkan_backend.c
 * @brief Implementation of the Vulkan renderer backend.
 *
 * This file provides concrete implementations of the renderer backend functions
 * using the Vulkan graphics API. Currently, it creates a Vulkan instance and sets
 * up basic placeholder logic for frame handling and resizing.
 */

// Static global context for the Vulkan renderer
static vulkan_context context;

b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, struct platform_state* plat_state) {
    // TODO: Implement support for custom allocators
    context.allocator = 0;

    // Application info for the Vulkan instance
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Koru Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    // Instance creation info
    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames = 0;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    // Create the Vulkan instance
    VkResult result = vkCreateInstance(&create_info, context.allocator, &context.instance);
    if (result != VK_SUCCESS) {
        KERROR("vkCreateInstance failed with result: %u", result);
        return FALSE;
    }

    KINFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(renderer_backend* backend) {
    // Destroy the Vulkan instance if it was created
    if (context.instance) {
        vkDestroyInstance(context.instance, context.allocator);
        context.instance = 0;
    }

    // Clear the allocator
    context.allocator = 0;

    KINFO("Vulkan renderer shut down successfully.");
}

void vulkan_renderer_backend_on_resized(renderer_backend* backend, u16 width, u16 height) {
    // Placeholder — actual implementation will recreate swapchain or update viewport
    KTRACE("Vulkan backend resized to %ux%u.", width, height);
}

b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 delta_time) {
    // Placeholder — actual implementation will begin frame operations
    return TRUE;
}

b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 delta_time) {
    // Placeholder — actual implementation will end frame and present image
    return TRUE;
}