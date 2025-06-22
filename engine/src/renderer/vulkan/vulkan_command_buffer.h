#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_command_buffer.h
 * @brief Vulkan command buffer management interface for the Koru Engine.
 *
 * This module provides functions to:
 * - Allocate and free command buffers from a pool
 * - Begin and end command recording
 * - Submit buffers to the GPU
 * - Manage state transitions during the command buffer lifecycle
 *
 * Command buffers store GPU commands like drawing, memory transfers,
 * and render pass operations. They must be allocated from a command pool,
 * recorded into, and submitted to a queue for execution.
 */

/**
 * @brief Allocates a new command buffer from the given command pool.
 *
 * Initializes the command buffer and sets its state to READY.
 *
 * @param context A pointer to the active Vulkan context.
 * @param pool The command pool to allocate from.
 * @param is_primary TRUE if primary command buffer, FALSE for secondary.
 * @param out_command_buffer A pointer to a vulkan_command_buffer to populate.
 */
void vulkan_command_buffer_allocate(
    vulkan_context* context,
    VkCommandPool pool,
    b8 is_primary,
    vulkan_command_buffer* out_command_buffer);

/**
 * @brief Frees the command buffer back to the pool.
 *
 * Must be called before destroying the pool or logical device.
 *
 * @param context A pointer to the active Vulkan context.
 * @param pool The command pool that owns the buffer.
 * @param command_buffer A pointer to the vulkan_command_buffer to free.
 */
void vulkan_command_buffer_free(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* command_buffer);

/**
 * @brief Begins recording commands into the command buffer.
 *
 * Sets up flags like one-time use or simultaneous access.
 *
 * @param command_buffer A pointer to the command buffer being recorded.
 * @param is_single_use TRUE if this buffer will only be used once.
 * @param is_renderpass_continue TRUE if this is a secondary buffer continuing a render pass.
 * @param is_simultaneous_use TRUE if the buffer can be re-recorded while in flight.
 */
void vulkan_command_buffer_begin(
    vulkan_command_buffer* command_buffer,
    b8 is_single_use,
    b8 is_renderpass_continue,
    b8 is_simultaneous_use);

/**
 * @brief Ends the recording phase of a command buffer.
 *
 * After calling this, the command buffer is ready for submission.
 *
 * @param command_buffer A pointer to the command buffer being ended.
 */
void vulkan_command_buffer_end(vulkan_command_buffer* command_buffer);

/**
 * @brief Updates the state of the command buffer to SUBMITTED.
 *
 * Called after submitting the buffer to a queue.
 *
 * @param command_buffer A pointer to the command buffer to mark as submitted.
 */
void vulkan_command_buffer_update_submitted(vulkan_command_buffer* command_buffer);

/**
 * @brief Resets the command buffer state to READY.
 *
 * Allows reuse of the command buffer after it has been submitted.
 *
 * @param command_buffer A pointer to the command buffer to reset.
 */
void vulkan_command_buffer_reset(vulkan_command_buffer* command_buffer);

/**
 * @brief Allocates and begins recording a single-use command buffer.
 *
 * Combines allocate and begin steps for convenience.
 * Typically used for quick GPU tasks like texture uploads.
 *
 * @param context A pointer to the active Vulkan context.
 * @param pool The command pool to allocate from.
 * @param out_command_buffer A pointer to the vulkan_command_buffer to initialize.
 */
void vulkan_command_buffer_allocate_and_begin_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* out_command_buffer);

/**
 * @brief Ends and submits a single-use command buffer.
 *
 * Submits the buffer to the provided queue and waits for completion.
 * Then frees the buffer back to the pool.
 *
 * @param context A pointer to the active Vulkan context.
 * @param pool The command pool that owns the buffer.
 * @param command_buffer A pointer to the command buffer to submit.
 * @param queue The queue to submit the buffer to.
 */
void vulkan_command_buffer_end_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* command_buffer,
    VkQueue queue);