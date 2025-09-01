#pragma once

#include "renderer/vulkan/vulkan_types.inl"
#include "renderer/renderer_types.inl"

/**
 *
 * @file vulkan_object_shader.h
 *
 * @brief Header for the Vulkan object shader implementation.
 *
 * Defines the structure and functions for managing a Vulkan shader used for rendering 3D objects.
 * This includes shader stages, descriptor sets, uniform buffers, and the graphics pipeline.
 */

/**
 * @brief Creates a Vulkan object shader for rendering 3D objects.
 *
 * This function initializes the shader stages, descriptor sets, uniform buffers,
 * and graphics pipeline needed for rendering objects in a Vulkan context.
 *
 * @param context The Vulkan context containing device and swapchain information.
 * @param out_shader Pointer to the vulkan_object_shader structure to be initialized.
 * @return True if the shader was created successfully; otherwise False.
 */
b8 vulkan_object_shader_create(vulkan_context* context, vulkan_object_shader* out_shader);

/**
 * @brief Destroys a Vulkan object shader and frees associated resources.
 *
 * This function cleans up all resources allocated for the object shader,
 * including shader modules, descriptor sets, uniform buffers, and the graphics pipeline.
 *
 * @param context The Vulkan context containing device information.
 * @param shader Pointer to the vulkan_object_shader structure to be destroyed.
 */
void vulkan_object_shader_destroy(vulkan_context* context, struct vulkan_object_shader* shader);

/**
 * @brief Binds the Vulkan object shader's pipeline for rendering.
 *
 * This function binds the graphics pipeline associated with the object shader
 * to the command buffer for the current frame, preparing it for drawing operations.
 *
 * @param context The Vulkan context containing command buffers and frame information.
 * @param shader Pointer to the vulkan_object_shader structure whose pipeline is to be bound.
 */
void vulkan_object_shader_use(vulkan_context* context, struct vulkan_object_shader* shader);

/**
 * @brief Updates the global uniform state for the Vulkan object shader.
 *
 * This function updates the global uniform buffer object (UBO) with the current
 * projection and view matrices, and binds the appropriate descriptor set for the current frame.
 *
 * @param context The Vulkan context containing command buffers and frame information.
 * @param shader Pointer to the vulkan_object_shader structure whose global state is to be updated.
 */
void vulkan_object_shader_update_global_state(vulkan_context* context, struct vulkan_object_shader* shader);

/**
 * @brief Updates the model matrix for a specific object in the Vulkan object shader.
 *
 * This function uses push constants to update the model transformation matrix
 * for a specific object being rendered, allowing for efficient per-object transformations.
 *
 * @param context The Vulkan context containing command buffers and frame information.
 * @param shader Pointer to the vulkan_object_shader structure whose object state is to be updated.
 * @param model The model transformation matrix for the object.
 */
void vulkan_object_shader_update_object(vulkan_context* context, struct vulkan_object_shader* shader, mat4 model);