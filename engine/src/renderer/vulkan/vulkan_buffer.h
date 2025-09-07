#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_buffer.h
 * @brief Vulkan buffer management interface for the Koru Engine.
 *
 * This module provides functions to:
 * - Create and destroy Vulkan buffers
 * - Lock and unlock buffer memory for CPU access
 * - Load data into buffers
 * - Copy data between buffers
 *
 * Buffers are used to store vertex data, index data, uniform data, and other GPU resources.
 * Proper management of buffer memory is crucial for performance and correctness in Vulkan applications.
 */

/**
 * @brief Creates a Vulkan buffer with the specified parameters.
 * The buffer can be optionally bound to memory upon creation.
 *
 * @param context A pointer to the active Vulkan context.
 * @param size The size of the buffer in bytes.
 * @param usage Usage flags indicating how the buffer will be used (e.g., VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).
 * @param memory_property_flags Memory property flags indicating how the buffer's memory can be accessed (e.g., VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT).
 * @param bind_on_create If True, the buffer will be bound to memory immediately after creation.
 * @param out_buffer A pointer to a vulkan_buffer struct to populate with the created buffer's details.
 * @return True if the buffer was created successfully, False otherwise.
 */
b8 vulkan_buffer_create(
    vulkan_context* context,
    u64 size,
    VkBufferUsageFlagBits usage,
    u32 memory_property_flags,
    b8 bind_on_create,
    vulkan_buffer* out_buffer);

/**
 * @brief Destroys a Vulkan buffer and frees its associated memory.
 * Must be called before destroying the Vulkan device or context.
 *
 * @param context A pointer to the active Vulkan context.
 * @param buffer A pointer to the vulkan_buffer to destroy.
 */
void vulkan_buffer_destroy(vulkan_context* context, vulkan_buffer* buffer);

/**
 * @brief Locks a region of the buffer's memory for CPU access.
 * Returns a pointer to the mapped memory region.
 *
 * @param context A pointer to the active Vulkan context.
 * @param buffer A pointer to the vulkan_buffer to lock.
 * @param offset The byte offset within the buffer to start locking.
 * @param size The size in bytes of the region to lock.
 * @param flags Flags for memory mapping (e.g., 0 for default behavior).
 */
void* vulkan_buffer_lock_memory(vulkan_context* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags);

/**
 * @brief Unlocks a previously locked region of the buffer's memory.
 * Must be called after finishing CPU access to the memory.
 *
 * @param context A pointer to the active Vulkan context.
 * @param buffer A pointer to the vulkan_buffer to unlock.
 */
void vulkan_buffer_unlock_memory(vulkan_context* context, vulkan_buffer* buffer);

/**
 * @brief Binds the buffer to its allocated memory at the specified offset.
 * Must be called before using the buffer in commands.
 *
 * @param context A pointer to the active Vulkan context.
 * @param buffer A pointer to the vulkan_buffer to bind.
 * @param offset The byte offset within the memory to bind the buffer to.
 */
void vulkan_buffer_bind(vulkan_context* context, vulkan_buffer* buffer, u64 offset);

/**
 * @brief Loads data into the buffer at the specified offset.
 * The buffer must be created with memory properties that allow CPU access.
 *
 * @param context A pointer to the active Vulkan context.
 * @param buffer A pointer to the vulkan_buffer to load data into.
 * @param offset The byte offset within the buffer to start loading data.
 * @param size The size in bytes of the data to load.
 * @param flags Flags for memory operations (e.g., 0 for default behavior).
 * @param data A pointer to the source data to copy into the buffer.
 */
void vulkan_buffer_load_data(vulkan_context* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags, const void* data);

/**
 * @brief Copies data from one buffer to another using a command buffer.
 * Both buffers must be created with appropriate usage flags to allow copying.
 *
 * @param context A pointer to the active Vulkan context.
 * @param pool The command pool to allocate the copy command buffer from.
 * @param fence A fence to signal when the copy operation is complete.
 * @param queue The queue to submit the copy command buffer to.
 * @param source The source buffer to copy data from.
 * @param source_offset The byte offset within the source buffer to start copying from.
 * @param dest The destination buffer to copy data to.
 * @param dest_offset The byte offset within the destination buffer to start copying to.
 * @param size The size in bytes of the data to copy.
 */
void vulkan_buffer_copy_to(
    vulkan_context* context,
    VkCommandPool pool,
    VkFence fence,
    VkQueue queue,
    VkBuffer source,
    u64 source_offset,
    VkBuffer dest,
    u64 dest_offset,
    u64 size);