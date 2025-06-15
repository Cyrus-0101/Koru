#include "containers/darray.h"
#include "core/application.h"
#include "core/logger.h"
#include "core/kstring.h"
#include "core/kmemory.h"
#include "platform/platform.h"
#include "vulkan_backend.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_fence.h"
#include "vulkan_framebuffer.h"
#include "vulkan_platform.h"
#include "vulkan_renderpass.h"
#include "vulkan_swapchain.h"
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

// Static global cached_framebuffer_width
static u32 cached_framebuffer_width = 0;

// Static global cached_framebuffer_height
static u32 cached_framebuffer_height = 0;

/**
 * @brief Debug messenger callback for Vulkan validation layers.
 *
 * Logs debug messages from the Vulkan validation layers to the engine's logging system.
 *
 * @param message_severity The severity of the message (error/warning/info/verbose).
 * @param message_types The type(s) of message (validation/performance/general).
 * @param callback_data Additional data about the message.
 * @param user_data Optional user-defined data (not used here).
 * @return Always returns VK_FALSE (no need to abort due to message).
 */
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

/**
 * @brief Finds a suitable memory type index that satisfies the given constraints.
 *
 * This function queries the physical device's memory properties and looks for a memory type that:
 * - Is supported by the filter (`type_filter`)
 * - Has all required property flags (`property_flags`)
 *
 * Used when allocating buffers and images to ensure they are placed in memory that is:
 * - Device-local (fast GPU access)
 * - Host-visible (can be accessed from CPU)
 * - Coherent (CPU/GPU cache behavior is consistent)
 *
 * If no matching memory type is found, logs a warning and returns -1.
 *
 * @param type_filter A bitmask indicating valid memory types (from VkMemoryRequirements.memoryTypeBits).
 * @param property_flags Required memory property flags (e.g., VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).
 * @return i32 The index of a suitable memory type, or -1 if none was found.
 */
i32 find_memory_index(u32 type_filter, u32 property_flags);

/**
 * @brief Creates and allocates command buffers for each frame-in-flight.
 *
 * Allocates one command buffer per swapchain image using the graphics command pool.
 * These buffers are used during rendering to record draw commands and submit them to the GPU.
 *
 * This function also ensures:
 * - All command buffers are zeroed before use
 * - Command buffers are created only once
 * - They are properly indexed and accessible globally via context
 *
 * @param backend A pointer to the renderer backend interface (unused here, but kept for consistency).
 */
void create_command_buffers(renderer_backend* backend);

/**
 * @brief Regenerates all framebuffers used in rendering after swapchain creation or resize.
 *
 * Framebuffers are tightly coupled to the swapchain — every time the swapchain is created,
 * recreated (due to window resize), or invalidated, all associated framebuffers must be regenerated.
 *
 * This function:
 * - Destroys any existing framebuffers
 * - Creates new ones using the current swapchain images and render pass
 * - Binds depth/stencil attachments as needed
 *
 * Must be called after:
 * - Swapchain creation
 * - Render pass creation
 * - Depth attachment setup
 *
 * @param backend A pointer to the renderer backend interface.
 * @param swapchain A pointer to the vulkan_swapchain being used for rendering.
 * @param renderpass A pointer to the vulkan_renderpass that defines how rendering happens.
 */
void regenerate_framebuffers(renderer_backend* backend, vulkan_swapchain* swapchain, vulkan_renderpass* renderpass);

b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, struct platform_state* plat_state) {
    KINFO("Creating Vulkan instance...");
    // Function pointers
    context.find_memory_index = find_memory_index;

    // TODO: Implement support for custom allocators
    context.allocator = 0;

    application_get_framebuffer_size(&cached_framebuffer_width, &cached_framebuffer_height);
    context.framebuffer_width = (cached_framebuffer_width != 0) ? cached_framebuffer_width : 800;
    context.framebuffer_height = (cached_framebuffer_height != 0) ? cached_framebuffer_height : 600;
    cached_framebuffer_width = 0;
    cached_framebuffer_height = 0;

    // Application info for the Vulkan instance
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 313);
    app_info.pApplicationName = application_name;
    // VK_MAKE_API_VERSION(major, minor, patch, variant)
    app_info.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    app_info.pEngineName = "Koru Engine";
    app_info.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);

    // Instance creation info
    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;

    // Obtain a list of required extensions
    const char** required_extensions = darray_create(const char*);

    darray_push(required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME);  // Generic surface extension

    platform_get_required_extension_names(&required_extensions);  // Platform-specific extension(s)

#if defined(_DEBUG)
    darray_push(required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // debug utilities

    KDEBUG("Required extensions:");

    u32 length = darray_length(required_extensions);

    for (u32 i = 0; i < length; ++i) {
        KDEBUG(required_extensions[i]);
    }
#endif

    create_info.enabledExtensionCount = darray_length(required_extensions);

    create_info.ppEnabledExtensionNames = required_extensions;

    // Validation layers.
    const char** required_validation_layer_names = 0;
    u32 required_validation_layer_count = 0;

// If validation should be done, get a list of the required validation layert names
// and make sure they exist. Validation layers should only be enabled on non-release builds.
#if defined(_DEBUG)
    KINFO("Validation layers enabled. Enumerating...");

    // The list of validation layers required.
    required_validation_layer_names = darray_create(const char*);
    darray_push(required_validation_layer_names, &"VK_LAYER_KHRONOS_validation");
    required_validation_layer_count = darray_length(required_validation_layer_names);

    // Obtain a list of available validation layers
    u32 available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
    VkLayerProperties* available_layers = darray_reserve(VkLayerProperties, available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));

    // Verify all required layers are available.
    for (u32 i = 0; i < required_validation_layer_count; ++i) {
        KINFO("Searching for layer: %s...", required_validation_layer_names[i]);
        b8 found = FALSE;
        for (u32 j = 0; j < available_layer_count; ++j) {
            if (strings_equal(required_validation_layer_names[i], available_layers[j].layerName)) {
                found = TRUE;
                KINFO("Found.");
                break;
            }
        }

        if (!found) {
            KFATAL("Required validation layer is missing: %s", required_validation_layer_names[i]);
            return FALSE;
        }
    }
    KINFO("All required validation layers are present.");
#endif

    create_info.enabledLayerCount = required_validation_layer_count;
    create_info.ppEnabledLayerNames = required_validation_layer_names;

    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));
    KINFO("Vulkan Instance created.");

    // Debugger creation
#if defined(_DEBUG)
    KDEBUG("Creating Vulkan debugger...");

    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;  //|
                                                                      //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};

    debug_create_info.messageSeverity = log_severity;

    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

    debug_create_info.pfnUserCallback = vk_debug_callback;

    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");

    KASSERT_MSG(func, "Failed to create debug messenger!");

    VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));

    KDEBUG("Vulkan debugger created.");
#endif

    // Surface creation
    KDEBUG("Creating Vulkan surface...");

    if (!platform_create_vulkan_surface(plat_state, &context)) {
        KERROR("Failed to create platform surface!");
        return FALSE;
    }

    KDEBUG("Vulkan surface created.");

    // Device creation
    if (!vulkan_device_create(&context)) {
        KERROR("Failed to create device!");
        return FALSE;
    }

    // Swapchain Creation
    KDEBUG("Creating Vulkan Swapchain...");
    vulkan_swapchain_create(
        &context,
        context.framebuffer_width,
        context.framebuffer_height,
        &context.swapchain);

    // Renderpass Creation
    KDEBUG("Creating Vulkan Renderpass...");
    vulkan_renderpass_create(
        &context,
        &context.main_renderpass,
        0, 0, context.framebuffer_width, context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0);

    // Swapchain framebuffers.
    KDEBUG("Creating Vulkan Framebuffers...");
    context.swapchain.framebuffers = darray_reserve(vulkan_framebuffer, context.swapchain.image_count);
    regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

    // Command Buffer Creation
    KDEBUG("Creating Vulkan Command Buffers...");
    create_command_buffers(backend);

    // Create sync objects.
    KDEBUG("Allocating Vulkan Sync Objects...");
    context.image_available_semaphores = darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
    context.queue_complete_semaphores = darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
    context.in_flight_fences = darray_reserve(vulkan_fence, context.swapchain.max_frames_in_flight);

    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
        VkSemaphoreCreateInfo semaphore_create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        vkCreateSemaphore(context.device.logical_device, &semaphore_create_info, context.allocator, &context.image_available_semaphores[i]);
        vkCreateSemaphore(context.device.logical_device, &semaphore_create_info, context.allocator, &context.queue_complete_semaphores[i]);

        // Create the fence in a signaled state, indicating that the first frame has already been "rendered".
        // This will prevent the application from waiting indefinitely for the first frame to render since it
        // cannot be rendered until a frame is "rendered" before it.
        vulkan_fence_create(&context, TRUE, &context.in_flight_fences[i]);
    }

    // In flight fences should not yet exist at this point, so clear the list. These are stored in pointers
    // because the initial state should be 0, and will be 0 when not in use. Acutal fences are not owned
    // by this list.
    KDEBUG("Allocating Vulkan Fences...");
    context.images_in_flight = darray_reserve(vulkan_fence, context.swapchain.image_count);
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        context.images_in_flight[i] = 0;
    }

    KINFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(renderer_backend* backend) {
    KINFO("Shutting down Vulkan renderer...");
    vkDeviceWaitIdle(context.device.logical_device);

    KDEBUG("Destroying Vulkan Sync Objects...");
    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
        if (context.image_available_semaphores[i]) {
            vkDestroySemaphore(
                context.device.logical_device,
                context.image_available_semaphores[i],
                context.allocator);
            context.image_available_semaphores[i] = 0;
        }

        if (context.queue_complete_semaphores[i]) {
            vkDestroySemaphore(
                context.device.logical_device,
                context.queue_complete_semaphores[i],
                context.allocator);
            context.queue_complete_semaphores[i] = 0;
        }

        KDEBUG("Destroying Vulkan Fences...");
        vulkan_fence_destroy(&context, &context.in_flight_fences[i]);
    }

    darray_destroy(context.image_available_semaphores);
    context.image_available_semaphores = 0;

    darray_destroy(context.queue_complete_semaphores);
    context.queue_complete_semaphores = 0;

    darray_destroy(context.in_flight_fences);
    context.in_flight_fences = 0;

    darray_destroy(context.images_in_flight);
    context.images_in_flight = 0;

    KDEBUG("Destroying Vulkan Command Buffers...");

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        if (context.graphics_command_buffers[i].handle) {
            vulkan_command_buffer_free(
                &context,
                context.device.graphics_command_pool,
                &context.graphics_command_buffers[i]);
            context.graphics_command_buffers[i].handle = 0;
        }
    }

    darray_destroy(context.graphics_command_buffers);

    context.graphics_command_buffers = 0;

    KDEBUG("Destroying Vulkan Framebuffers...");
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        vulkan_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    KDEBUG("Destroying Vulkan Renderpass...");
    vulkan_renderpass_destroy(&context, &context.main_renderpass);

    KDEBUG("Destroying Vulkan Swapchain...");
    vulkan_swapchain_destroy(&context, &context.swapchain);

    KDEBUG("Destroying logical device...");
    vulkan_device_destroy(&context);

    KDEBUG("Destroying Vulkan Surface...");
    vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
    context.surface = 0;

#if defined(_DEBUG)
    KDEBUG("Destroying Vulkan Debugger...");
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");

        if (func) {
            func(context.instance, context.debug_messenger, context.allocator);
        }
    }
#endif

    KDEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
    context.instance = 0;
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

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            KERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            KWARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            KINFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            KTRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}

i32 find_memory_index(u32 type_filter, u32 property_flags) {
    KDEBUG("Finding memory index for filter %u, flags %u...", type_filter, property_flags);

    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physical_device, &memory_properties);

    for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
        // Check each memory type to see if its bit is set to 1.
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
            KDEBUG("Found valid memory index: %u", i);
            return i;
        }
    }

    KWARN("Unable to find suitable memory type!");
    return -1;
}

void create_command_buffers(renderer_backend* backend) {
    KDEBUG("Allocating %u command buffers...", context.swapchain.image_count);

    if (!context.graphics_command_buffers) {
        context.graphics_command_buffers = darray_reserve(vulkan_command_buffer, context.swapchain.image_count);
        for (u32 i = 0; i < context.swapchain.image_count; ++i) {
            kzero_memory(&context.graphics_command_buffers[i], sizeof(vulkan_command_buffer));
        }
    }

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        if (context.graphics_command_buffers[i].handle) {
            vulkan_command_buffer_free(
                &context,
                context.device.graphics_command_pool,
                &context.graphics_command_buffers[i]);
        }
        kzero_memory(&context.graphics_command_buffers[i], sizeof(vulkan_command_buffer));
        vulkan_command_buffer_allocate(
            &context,
            context.device.graphics_command_pool,
            TRUE,
            &context.graphics_command_buffers[i]);
    }

    KDEBUG("Vulkan command buffers allocated and reset.");
}

void regenerate_framebuffers(renderer_backend* backend, vulkan_swapchain* swapchain, vulkan_renderpass* renderpass) {
    for (u32 i = 0; i < swapchain->image_count; ++i) {
        // TODO: make this dynamic based on the currently configured attachments
        u32 attachment_count = 2;
        VkImageView attachments[] = {
            swapchain->views[i],
            swapchain->depth_attachment.view};

        vulkan_framebuffer_create(
            &context,
            renderpass,
            context.framebuffer_width,
            context.framebuffer_height,
            attachment_count,
            attachments,
            &context.swapchain.framebuffers[i]);
    }
}