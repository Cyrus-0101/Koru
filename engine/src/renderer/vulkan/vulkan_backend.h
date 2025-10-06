#pragma once

#include "renderer/renderer_backend.h"
#include "resources/resource_types.h"
#include "vulkan_types.inl"

/**
 * @file vulkan_backend.h
 * @brief Public interface for the Vulkan renderer backend implementation.
 *
 * This module implements the function table required by the renderer_frontend,
 * providing Vulkan-specific implementations for initialization, shutdown,
 * frame handling, and window resize handling.
 */

/**
 * @brief Debug messenger callback for Vulkan validation layers.
 *
 * Logs debug messages from the Vulkan validation layers to the engine's logging system.
 *
 * @param message_severity The severity of the message (error/warning/info/verbose).
 * @param message_types The type(s) of message (validation/performance/general).
 * @param callback_data Additional data about the message.
 * @param user_data Optional user-defined data (not used here).
 * @return Always returns VK_False (no need to abort due to message).
 */
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

/** * @brief Creates the necessary Vulkan buffers for rendering objects.
 *
 * This function initializes the vertex and index buffers used for rendering 3D objects.
 * It sets up the buffers with appropriate usage flags and memory properties.
 *
 * The vertex buffer is used to store vertex data, while the index buffer is used for indexed drawing.
 * Both buffers are created with device-local memory for optimal performance.
 *
 * @param context The Vulkan context containing device and memory information.
 * @return True if buffers were created successfully, false otherwise.
 */
b8 create_buffers(vulkan_context* context);

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

/**
 * @brief Recreates the swapchain and associated resources.
 *
 * This function is called when the window is resized or when the swapchain needs to be recreated
 * for any reason (e.g., device loss). It handles the entire process of destroying the old swapchain
 * and creating a new one, along with all associated resources (framebuffers, command buffers, etc.).
 *
 * @param backend A pointer to the renderer backend interface.
 * @return True if the swapchain was successfully recreated, false otherwise.
 */
b8 recreate_swapchain(renderer_backend* backend);

/**
 * @brief Uploads data to a specified range of a Vulkan buffer.
 *
 * This function creates a staging buffer, uploads the data to it, and then copies it to the
 * specified Vulkan buffer.
 *
 * @param context The Vulkan context.
 * @param pool The command pool to use for buffer operations.
 * @param fence The fence to signal when the operation is complete.
 * @param queue The queue to submit the copy command to.
 * @param buffer The Vulkan buffer to upload data to.
 * @param offset The offset within the buffer to upload data to.
 * @param size The size of the data to upload.
 * @param data The data to upload.
 */
void upload_data_range(vulkan_context* context,
                       VkCommandPool pool,
                       VkFence fence,
                       VkQueue queue,
                       vulkan_buffer* buffer,
                       u64 offset,
                       u64 size,
                       void* data);

/**
 * @brief Initializes the Vulkan renderer backend.
 *
 * Creates the Vulkan instance and prepares internal resources needed for rendering.
 *
 * @param backend A pointer to the renderer backend structure.
 * @param application_name Name of the application using the renderer.
 * @param plat_state A pointer to the platform-specific state.
 * @return True if successful; otherwise False.
 */
b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name);

/**
 * @brief Shuts down the Vulkan renderer backend.
 *
 * Cleans up all resources allocated during initialization.
 *
 * @param backend A pointer to the renderer backend structure.
 */
void vulkan_renderer_backend_shutdown(renderer_backend* backend);

/**
 * @brief Handles a window resize event for the Vulkan backend.
 *
 * May involve recreating the swapchain or adjusting viewport settings.
 *
 * @param backend A pointer to the renderer backend structure.
 * @param width New width of the window.
 * @param height New height of the window.
 */
void vulkan_renderer_backend_on_resized(renderer_backend* backend, u16 width, u16 height);

/**
 * @brief Begins a new rendering frame in the Vulkan backend.
 *
 * Sets up command buffers, render passes, etc.
 *
 * @param backend A pointer to the renderer backend structure.
 * @param delta_time Time in seconds since the last frame.
 * @return True if successful; otherwise False.
 */
b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 delta_time);

/**
 * @brief Ends the current rendering frame in the Vulkan backend.
 *
 * Submits command buffers, presents the image to the screen, and cleans up per-frame resources.
 *
 * @param backend A pointer to the renderer backend structure.
 * @param delta_time Time in seconds since the last frame.
 * @return True if successful; otherwise False.
 */
b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 delta_time);

/**
 * @brief Updates the global state for rendering in the Vulkan backend.
 *
 * This function is called each frame to update the global uniform object
 * used for rendering, such as projection and view matrices.
 *
 * @param projection The projection matrix for the current frame.
 * @param view The view matrix for the current frame.
 * @param view_position The camera's position in world space.
 * @param ambient_colour The ambient light colour in RGBA format.
 * @param mode Rendering mode (e.g., wireframe, solid).
 * @return void
 */
void vulkan_renderer_update_global_state(mat4 projection, mat4 view, vec3 view_position, vec4 ambient_colour, i32 mode);

/**
 * @brief Updates the model matrix for a specific object in the Vulkan backend.
 *
 * @param model The model matrix for the object.
 * @return void
 */
void vulkan_backend_update_object(geometry_render_data data);

/**
 * @brief Creates a texture in the Vulkan renderer backend.
 *
 * This function creates a texture from pixel data and initializes it for use in rendering.
 *
 * @param name The name of the texture (for debugging purposes).
 * @param width The width of the texture in pixels.
 * @param height The height of the texture in pixels.
 * @param channel_count The number of channels in the texture (e.g., 4 for RGBA).
 * @param pixels Pointer to the pixel data to upload to the texture.
 * @param has_transparency Whether the texture has an alpha channel for transparency.
 * @param out_texture Pointer to the texture structure to fill with the created texture.
 */
void vulkan_renderer_create_texture(const char* name, i32 width, i32 height, i32 channel_count, const u8* pixels, b8 has_transparency, texture* out_texture);

/**
 * @brief Destroys a texture in the Vulkan renderer backend.
 *
 * Cleans up all resources associated with the texture, including Vulkan image and sampler.
 *
 * @param texture Pointer to the texture structure to destroy.
 */
void vulkan_renderer_destroy_texture(texture* texture);

/**
 * @brief Creates a material resource.
 *
 * This function initializes a material with the provided properties
 * and prepares it for use in rendering operations.
 *
 * @param material Pointer to the material structure to be filled out.
 * @return True if the material was created successfully; otherwise False.
 */
b8 vulkan_renderer_create_material(struct material* material);

/**
 * @brief Destroys a material resource and frees associated resources.
 *
 * @param material Pointer to the material to be destroyed.
 */
void vulkan_renderer_destroy_material(struct material* material);