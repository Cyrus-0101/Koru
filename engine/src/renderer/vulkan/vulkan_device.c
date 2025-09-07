#include "containers/darray.h"
#include "core/logger.h"
#include "core/kstring.h"
#include "core/kmemory.h"
#include "vulkan_device.h"

#define MAX_DEVICE_COUNT 32

/**
 * @file vulkan_device.c
 * @brief Implementation of Vulkan device creation and selection logic.
 *
 * This module provides functions to:
 * - Enumerate and select a suitable physical device
 * - Query device capabilities and queue families
 * - Check for required features (e.g., swapchain, anisotropic filtering)
 * - Create a logical device with necessary queues
 */

/**
 * @struct vulkan_physical_device_requirements
 *
 * @brief Describes required features of a physical device.
 *
 * Used during device selection to filter out unsuitable GPUs.
 */
typedef struct vulkan_physical_device_requirements {
    /**
     * @brief Device must support graphics queues.
     */
    b8 graphics;

    /**
     * @brief Device must support present queues.
     */
    b8 present;

    /**
     * @brief Device must support compute queues.
     */
    b8 compute;

    /**
     * @brief Device must support transfer queues.
     */
    b8 transfer;

    /**
     * @brief Required device extensions (e.g., VK_KHR_SWAPCHAIN_EXTENSION_NAME).
     */
    const char** device_extension_names;

    /**
     * @brief Must support sampler anisotropy.
     */
    b8 sampler_anisotropy;

    /**
     * @brief Must be a discrete GPU (if enabled).
     */
    b8 discrete_gpu;
} vulkan_physical_device_requirements;

/**
 * @struct vulkan_physical_device_queue_family_info
 * @brief Stores indices of queue families for a physical device.
 *
 * Used to record which queue families support:
 * - Graphics
 * - Presentation
 * - Compute
 * - Transfer operations
 */
typedef struct vulkan_physical_device_queue_family_info {
    /**
     * @brief Index of the queue family that supports graphics operations.
     */
    u32 graphics_family_index;

    /**
     * @brief Index of the queue family that supports presentation to the window.
     */
    u32 present_family_index;

    /**
     * @brief Index of the queue family that supports compute operations.
     */
    u32 compute_family_index;

    /**
     * @brief Index of the queue family that supports transfer operations.
     */
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;

/**
 * @brief Selects the most suitable physical device from the list of available devices.
 *
 * Iterates through all available physical devices and checks whether each meets the requirements.
 * If a suitable device is found, it is stored in the context.
 *
 * Requirements include:
 * - Support for graphics, presentation, and transfer queues
 * - Required extensions (like VK_KHR_swapchain)
 * - Optional features like anisotropic filtering
 * - Optional preference for discrete GPU
 *
 * @param context A pointer to the Vulkan context where device info will be stored.
 * @return True if a suitable device was found and selected; False otherwise.
 */
b8 select_physical_device(vulkan_context* context);

/**
 * @brief Checks whether the given physical device meets all specified requirements.
 *
 * Evaluates:
 * - Queue family support (graphics, present, compute, transfer)
 * - Surface/swapchain capabilities
 * - Required extensions
 * - Feature support (like samplerAnisotropy)
 *
 * @param device The physical device to evaluate.
 * @param surface The surface to test for presentation support.
 * @param properties Device properties (name, type, driver version).
 * @param features Device features (supports what?).
 * @param requirements Required features and capabilities.
 * @param out_queue_family_info Output structure for queue family indices.
 * @param out_swapchain_support Output structure for swapchain support data.
 * @return True if device meets all requirements; False otherwise.
 */
b8 physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_family_info,
    vulkan_swapchain_support_info* out_swapchain_support);

/**
 * @brief Creates and selects a suitable physical/logical Vulkan device.
 *
 * Queries available physical devices, selects one based on feature and queue support,
 * creates a logical device, and initializes queues (graphics, present, compute, transfer).
 *
 * @param context A pointer to the Vulkan context. Must be valid and initialized.
 * @return True if a suitable device was found and successfully created; False otherwise.
 */
b8 vulkan_device_create(vulkan_context* context) {
    if (!select_physical_device(context)) {
        return False;
    }

    KINFO("Creating logical device...");

    // NOTE: Do not create additional queues for shared indices.
    b8 present_shares_graphics_queue = context->device.graphics_queue_index == context->device.present_queue_index;
    b8 transfer_shares_graphics_queue = context->device.graphics_queue_index == context->device.transfer_queue_index;
    u32 index_count = 1;

    if (!present_shares_graphics_queue) {
        index_count++;
    }

    if (!transfer_shares_graphics_queue) {
        index_count++;
    }

    u32 indices[32];
    u8 index = 0;
    indices[index++] = context->device.graphics_queue_index;

    if (!present_shares_graphics_queue) {
        indices[index++] = context->device.present_queue_index;
    }

    if (!transfer_shares_graphics_queue) {
        indices[index++] = context->device.transfer_queue_index;
    }

    VkDeviceQueueCreateInfo queue_create_infos[32];
    for (u32 i = 0; i < index_count; ++i) {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount = 1;
        // if (indices[i] == context->device.graphics_queue_index) {
        //     queue_create_infos[i].queueCount = 1;  ///> Always use 1 unless you know better
        // }
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;
        f32 queue_priority = 1.0f;
        queue_create_infos[i].pQueuePriorities = &queue_priority;
    }

    // Request device features.
    // TODO: should be config driven
    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;  // Request anisotropy

    VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.queueCreateInfoCount = index_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 1;
    const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    device_create_info.ppEnabledExtensionNames = &extension_names;

    // Deprecated and ignored, so pass nothing.
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = 0;

    // Create the device.
    VK_CHECK(vkCreateDevice(
        context->device.physical_device,
        &device_create_info,
        context->allocator,
        &context->device.logical_device));

    KINFO("Logical device created.");

    // Get queues.
    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.graphics_queue_index,
        0,
        &context->device.graphics_queue);

    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.present_queue_index,
        0,
        &context->device.present_queue);

    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.transfer_queue_index,
        0,
        &context->device.transfer_queue);
    KINFO("Queues obtained.");

    // Create command pool for graphics queue.
    KINFO("Creating command pools and buffers...");
    VkCommandPoolCreateInfo pool_create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_create_info.queueFamilyIndex = context->device.graphics_queue_index;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CHECK(vkCreateCommandPool(
        context->device.logical_device,
        &pool_create_info,
        context->allocator,
        &context->device.graphics_command_pool));

    KINFO("Graphics command pool created.");

    return True;
}

void vulkan_device_destroy(vulkan_context* context) {
    // Step 1: Destroy command pools
    KDEBUG("Destroying command pools...");
    if (context->device.graphics_command_pool) {
        vkDestroyCommandPool(context->device.logical_device, context->device.graphics_command_pool, context->allocator);
        context->device.graphics_command_pool = 0;
    }

    // If we add more command pools (e.g., compute, transfer), destroy them here
    // if (context->device.compute_command_pool) {
    //     vkDestroyCommandPool(context->device.logical_device, context->device.compute_command_pool, context->allocator);
    //     context->device.compute_command_pool = 0;
    // }

    // Step 2: Unset queues
    context->device.graphics_queue = 0;
    context->device.present_queue = 0;
    context->device.transfer_queue = 0;

    // Step 3: Destroy logical device
    KDEBUG("Destroying logical device...");
    if (context->device.logical_device) {
        vkDestroyDevice(context->device.logical_device, context->allocator);
        context->device.logical_device = 0;
    }

    // Step 4: Clean up physical device metadata
    KDEBUG("Releasing physical device resources...");
    context->device.physical_device = 0;

    if (context->device.swapchain_support.formats) {
        kfree(
            context->device.swapchain_support.formats,
            sizeof(VkSurfaceFormatKHR) * context->device.swapchain_support.format_count,
            MEMORY_TAG_RENDERER);
        context->device.swapchain_support.formats = 0;
        context->device.swapchain_support.format_count = 0;
    }

    if (context->device.swapchain_support.present_modes) {
        kfree(
            context->device.swapchain_support.present_modes,
            sizeof(VkPresentModeKHR) * context->device.swapchain_support.present_mode_count,
            MEMORY_TAG_RENDERER);
        context->device.swapchain_support.present_modes = 0;
        context->device.swapchain_support.present_mode_count = 0;
    }

    kzero_memory(
        &context->device.swapchain_support.capabilities,
        sizeof(context->device.swapchain_support.capabilities));

    // Step 5: Reset queue indices
    context->device.graphics_queue_index = -1;
    context->device.present_queue_index = -1;
    context->device.transfer_queue_index = -1;

    // Optional: Reset other device fields
    kzero_memory(&context->device.properties, sizeof(context->device.properties));
    kzero_memory(&context->device.features, sizeof(context->device.features));
    kzero_memory(&context->device.memory, sizeof(context->device.memory));
    context->device.depth_format = VK_FORMAT_UNDEFINED;
}

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info* out_support_info) {
    // Surface capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device,
        surface,
        &out_support_info->capabilities));

    // Surface formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device,
        surface,
        &out_support_info->format_count,
        0));

    if (out_support_info->format_count != 0) {
        if (!out_support_info->formats) {
            out_support_info->formats = kallocate(sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device,
            surface,
            &out_support_info->format_count,
            out_support_info->formats));
    }

    // Present modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        surface,
        &out_support_info->present_mode_count,
        0));
    if (out_support_info->present_mode_count != 0) {
        if (!out_support_info->present_modes) {
            out_support_info->present_modes = kallocate(sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info->present_mode_count,
            out_support_info->present_modes));
    }
}

b8 vulkan_device_detect_depth_format(vulkan_device* device) {
    // Format candidates
    const u64 candidate_count = 3;
    VkFormat candidates[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT};

    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (u64 i = 0; i < candidate_count; ++i) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i], &properties);

        if ((properties.linearTilingFeatures & flags) == flags) {
            device->depth_format = candidates[i];
            return True;
        } else if ((properties.optimalTilingFeatures & flags) == flags) {
            device->depth_format = candidates[i];
            return True;
        }
    }

    return False;
}

b8 select_physical_device(vulkan_context* context) {
    KINFO("Selecting physical device...");
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, 0));

    if (physical_device_count == 0) {
        KFATAL("No devices which support Vulkan were found.");
        return False;
    }

    VkPhysicalDevice physical_devices[MAX_DEVICE_COUNT];
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices));

    for (u32 i = 0; i < physical_device_count; ++i) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        // Check if device supports local/host visible combo
        b8 supports_device_local_host_visible = False;

        for (u32 i = 0; i < memory.memoryTypeCount; ++i) {
            // Check each memory type to see if its bit is set to 1.
            if (((memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) &&
                ((memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)) {
                supports_device_local_host_visible = True;
                break;
            }
        }

        // TODO: These requirements should probably be driven by engine
        // configuration.
        vulkan_physical_device_requirements requirements = {};
        requirements.graphics = True;
        requirements.present = True;
        requirements.transfer = True;
        // NOTE: Enable this if compute will be required.
        requirements.compute = True;
        requirements.sampler_anisotropy = True;
        requirements.discrete_gpu = False;
        requirements.device_extension_names = darray_create(const char*);
        darray_push(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info = {};
        b8 result = physical_device_meets_requirements(
            physical_devices[i],
            context->surface,
            &properties,
            &features,
            &requirements,
            &queue_info,
            &context->device.swapchain_support);

        if (result) {
            KINFO("Selected device: '%s'.", properties.deviceName);
            // GPU type, etc.
            switch (properties.deviceType) {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    KINFO("GPU type is Unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    KINFO("GPU type is Integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    KINFO("GPU type is Discrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    KINFO("GPU type is Virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    KINFO("GPU type is CPU.");
                    break;
            }

            KINFO(
                "GPU Driver version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.driverVersion),
                VK_VERSION_MINOR(properties.driverVersion),
                VK_VERSION_PATCH(properties.driverVersion));

            // Vulkan API version.
            KINFO(
                "Vulkan API version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion));

            // Memory information
            for (u32 j = 0; j < memory.memoryHeapCount; ++j) {
                f32 memory_size_gib = (((f32)memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    KINFO("Local GPU memory: %.2f GiB", memory_size_gib);
                } else {
                    KINFO("Shared System memory: %.2f GiB", memory_size_gib);
                }
            }

            context->device.physical_device = physical_devices[i];
            context->device.graphics_queue_index = queue_info.graphics_family_index;
            context->device.present_queue_index = queue_info.present_family_index;
            context->device.transfer_queue_index = queue_info.transfer_family_index;
            // NOTE: set compute index here if needed.

            // Keep a copy of properties, features and memory info for later use.
            context->device.properties = properties;
            context->device.features = features;
            context->device.memory = memory;
            context->device.supports_device_local_host_visible = supports_device_local_host_visible;
            break;
        }
    }

    // Ensure a device was selected
    if (!context->device.physical_device) {
        KERROR("No physical devices were found which meet the requirements.");
        return False;
    }

    KINFO("Physical device selected.");
    return True;
}

b8 physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_info,
    vulkan_swapchain_support_info* out_swapchain_support) {
    // Evaluate device properties to determine if it meets the needs of our applcation.
    out_queue_info->graphics_family_index = -1;
    out_queue_info->present_family_index = -1;
    out_queue_info->compute_family_index = -1;
    out_queue_info->transfer_family_index = -1;

    // Discrete GPU?
    if (requirements->discrete_gpu) {
        if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            KINFO("Device is not a discrete GPU, and one is required. Skipping.");
            return False;
        }
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
    VkQueueFamilyProperties queue_families[32];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    // Look at each queue and see what queues it supports
    KINFO("Graphics | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255;
    for (u32 i = 0; i < queue_family_count; ++i) {
        u8 current_transfer_score = 0;

        // Graphics queue?
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_queue_info->graphics_family_index = i;
            ++current_transfer_score;
        }

        // Compute queue?
        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_info->compute_family_index = i;
            ++current_transfer_score;
        }

        // Transfer queue?
        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            // Take the index if it is the current lowest. This increases the
            // likelihood that it is a dedicated transfer queue.
            if (current_transfer_score <= min_transfer_score) {
                min_transfer_score = current_transfer_score;
                out_queue_info->transfer_family_index = i;
            }
        }

        // Present queue?
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
        if (supports_present) {
            out_queue_info->present_family_index = i;
        }
    }

    // Print out some info about the device
    KINFO("       %d |       %d |       %d |        %d | %s",
          out_queue_info->graphics_family_index != -1,
          out_queue_info->present_family_index != -1,
          out_queue_info->compute_family_index != -1,
          out_queue_info->transfer_family_index != -1,
          properties->deviceName);

    if (
        (!requirements->graphics || (requirements->graphics && out_queue_info->graphics_family_index != -1)) &&
        (!requirements->present || (requirements->present && out_queue_info->present_family_index != -1)) &&
        (!requirements->compute || (requirements->compute && out_queue_info->compute_family_index != -1)) &&
        (!requirements->transfer || (requirements->transfer && out_queue_info->transfer_family_index != -1))) {
        KINFO("Device meets queue requirements.");
        KTRACE("Graphics Family Index: %i", out_queue_info->graphics_family_index);
        KTRACE("Present Family Index:  %i", out_queue_info->present_family_index);
        KTRACE("Transfer Family Index: %i", out_queue_info->transfer_family_index);
        KTRACE("Compute Family Index:  %i", out_queue_info->compute_family_index);

        // Query swapchain support.
        vulkan_device_query_swapchain_support(
            device,
            surface,
            out_swapchain_support);

        if (out_swapchain_support->format_count < 1 || out_swapchain_support->present_mode_count < 1) {
            if (out_swapchain_support->formats) {
                kfree(out_swapchain_support->formats, sizeof(VkSurfaceFormatKHR) * out_swapchain_support->format_count, MEMORY_TAG_RENDERER);
            }
            if (out_swapchain_support->present_modes) {
                kfree(out_swapchain_support->present_modes, sizeof(VkPresentModeKHR) * out_swapchain_support->present_mode_count, MEMORY_TAG_RENDERER);
            }
            KINFO("Required swapchain support not present, skipping device.");
            return False;
        }

        // Device extensions.
        if (requirements->device_extension_names) {
            u32 available_extension_count = 0;
            VkExtensionProperties* available_extensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(
                device,
                0,
                &available_extension_count,
                0));
            if (available_extension_count != 0) {
                available_extensions = kallocate(sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(
                    device,
                    0,
                    &available_extension_count,
                    available_extensions));

                u32 required_extension_count = darray_length(requirements->device_extension_names);
                for (u32 i = 0; i < required_extension_count; ++i) {
                    b8 found = False;
                    for (u32 j = 0; j < available_extension_count; ++j) {
                        if (strings_equal(requirements->device_extension_names[i], available_extensions[j].extensionName)) {
                            found = True;
                            break;
                        }
                    }

                    if (!found) {
                        KINFO("Required extension not found: '%s', skipping device.", requirements->device_extension_names[i]);
                        kfree(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
                        return False;
                    }
                }
            }
            kfree(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
        }

        // Sampler anisotropy
        if (requirements->sampler_anisotropy && !features->samplerAnisotropy) {
            KINFO("Device does not support sampler_anisotropy, skipping.");
            return False;
        }

        // Device meets all requirements.
        return True;
    }

    return False;
}