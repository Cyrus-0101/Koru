#pragma once

#include "renderer/vulkan/vulkan_types.inl"
#include "renderer/renderer_types.inl"

/**
 *
 * @file vulkan_material_shader.h
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
 * @param out_shader Pointer to the vulkan_material_shader structure to be initialized.
 * @return True if the shader was created successfully; otherwise False.
 */
b8 vulkan_material_shader_create(vulkan_context* context, vulkan_material_shader* out_shader);

/**
 * @brief Destroys a Vulkan object shader and frees associated resources.
 *
 * This function cleans up all resources allocated for the object shader,
 * including shader modules, descriptor sets, uniform buffers, and the graphics pipeline.
 *
 * @param context The Vulkan context containing device information.
 * @param shader Pointer to the vulkan_material_shader structure to be destroyed.
 */
void vulkan_material_shader_destroy(vulkan_context* context, struct vulkan_material_shader* shader);

/**
 * @brief Binds the Vulkan object shader's pipeline for rendering.
 *
 * This function binds the graphics pipeline associated with the object shader
 * to the command buffer for the current frame, preparing it for drawing operations.
 *
 * @param context The Vulkan context containing command buffers and frame information.
 * @param shader Pointer to the vulkan_material_shader structure whose pipeline is to be bound.
 */
void vulkan_material_shader_use(vulkan_context* context, struct vulkan_material_shader* shader);

/**
 * @brief Updates the global uniform state for the Vulkan object shader.
 *
 * This function updates the global uniform buffer object (UBO) with the current
 * projection and view matrices, and binds the appropriate descriptor set for the current frame.
 *
 * @param context The Vulkan context containing command buffers and frame information.
 * @param shader Pointer to the vulkan_material_shader structure whose global state is to be updated.
 */
void vulkan_material_shader_update_global_state(vulkan_context* context, struct vulkan_material_shader* shader, f32 delta_time);

/**
 * @brief Sets the model matrix for the Vulkan object shader using push constants.
 *
 * This function updates the model matrix used for transforming vertices in the vertex shader.
 * It utilizes push constants for efficient updates without needing to modify uniform buffers.
 *
 * @param context The Vulkan context containing command buffers and frame information.
 * @param shader Pointer to the vulkan_material_shader structure whose model matrix is to be set.
 * @param model The model matrix to be applied to the shader.
 */
void vulkan_material_shader_set_model(vulkan_context* context, struct vulkan_material_shader* shader, mat4 model);

/**
 * @brief Updates per-object descriptor data (UBO + texture) and binds the descriptor set.
 *
 * This function updates the uniform buffer object (UBO) and texture bindings
 * for a specific object being rendered, and binds the appropriate descriptor set
 * for the current frame. It should be called once per object before drawing.
 *
 * @param context The Vulkan context containing command buffers and frame information.
 * @param shader Pointer to the vulkan_material_shader structure whose object state is to be updated.
 * @param data Structure containing the model matrix and material information for the object.
 */
void vulkan_material_shader_apply_material(vulkan_context* context, struct vulkan_material_shader* shader, material* material);

/**
 * @brief Acquires resources for rendering a new object with the Vulkan object shader.
 *
 * This function allocates a new object ID and initializes its descriptor sets
 * for use in rendering. It manages a free list of object states to track resource usage.
 *
 * @param context The Vulkan context containing device and swapchain information.
 * @param shader Pointer to the vulkan_material_shader structure from which to acquire resources.
 * @param material Pointer to the material associated with the object being rendered.
 * @return True if resources were successfully acquired; otherwise False.
 */
b8 vulkan_material_shader_acquire_resources(vulkan_context* context, struct vulkan_material_shader* shader, material* material);

/**
 * @brief Releases resources associated with a specific object ID in the Vulkan object shader.
 *
 * This function frees the descriptor sets and resets the state for the given object ID,
 * allowing its resources to be reused for future objects. It manages the free list of object states.
 *
 * @param context The Vulkan context containing device information.
 * @param shader Pointer to the vulkan_material_shader structure from which to release resources.
 * @param material Pointer to the material associated with the object being released.
 */
void vulkan_material_shader_release_resources(vulkan_context* context, struct vulkan_material_shader* shader, material* material);