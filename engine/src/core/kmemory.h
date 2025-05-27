#pragma once

#include "defines.h"

/**
 * @brief Memory allocation tag types.
 *
 * Used to categorize memory allocations for tracking, debugging, and profiling.
 * Each tag should be used consistently for similar kinds of allocations.
 */
typedef enum memory_tag {
    // For temporary use only. Should eventually be replaced with a proper tag.
    MEMORY_TAG_UNKNOWN,

    // Dynamic arrays (e.g., array_push)
    MEMORY_TAG_ARRAY,

    // Dynamic arrays that grow/shrink dynamically
    MEMORY_TAG_DARRAY,

    // Dictionary/Hash Table allocations
    MEMORY_TAG_DICT,

    // Ring queue structures
    MEMORY_TAG_RING_QUEUE,

    // Binary search trees
    MEMORY_TAG_BST,

    // String-related allocations
    MEMORY_TAG_STRING,

    // Application-level state
    MEMORY_TAG_APPLICATION,

    // Job system allocations
    MEMORY_TAG_JOB,

    // Texture data
    MEMORY_TAG_TEXTURE,

    // Material instances
    MEMORY_TAG_MATERIAL_INSTANCE,

    // Renderer-specific allocations
    MEMORY_TAG_RENDERER,

    // Game logic/state
    MEMORY_TAG_GAME,

    // Transform components
    MEMORY_TAG_TRANSFORM,

    // Entity objects
    MEMORY_TAG_ENTITY,

    // Scene graph entity nodes
    MEMORY_TAG_ENTITY_NODE,

    // Scene management
    MEMORY_TAG_SCENE,

    // Must be last -- represents total number of tags
    MEMORY_TAG_MAX_TAGS
} memory_tag;

/**
 * @brief Initializes the memory allocator subsystem.
 *
 * Sets internal counters to zero and prepares for tracking allocations.
 */
KAPI void initialize_memory();

/**
 * @brief Shuts down the memory allocator subsystem.
 *
 * Currently does nothing but may be extended to log final stats or validate leaks.
 */
KAPI void shutdown_memory();

/**
 * @brief Allocates memory with the given size and tag.
 *
 * @param size The number of bytes to allocate.
 * @param tag A memory_tag to classify this allocation.
 * @return A pointer to the allocated memory block.
 */
KAPI void* kallocate(u64 size, memory_tag tag);

/**
 * @brief Frees a previously allocated memory block.
 *
 * @param block Pointer to the memory block to free.
 * @param size Size of the memory block in bytes.
 * @param tag Tag used when originally allocating the block.
 */
KAPI void kfree(void* block, u64 size, memory_tag tag);

/**
 * @brief Fills the provided memory block with zeros.
 *
 * @param block Pointer to the memory block.
 * @param size Number of bytes to zero out.
 * @return Pointer to the zeroed memory block.
 */
KAPI void* kzero_memory(void* block, u64 size);

/**
 * @brief Copies memory from one location to another.
 *
 * @param dest Destination memory block.
 * @param source Source memory block.
 * @param size Number of bytes to copy.
 * @return Pointer to the destination memory block.
 */
KAPI void* kcopy_memory(void* dest, const void* source, u64 size);

/**
 * @brief Fills a memory block with a specific byte value.
 *
 * @param dest Destination memory block.
 * @param value Byte value to fill with.
 * @param size Number of bytes to fill.
 * @return Pointer to the filled memory block.
 */
KAPI void* kset_memory(void* dest, i32 value, u64 size);

/**
 * @brief Gets a formatted string showing current memory usage per tag.
 *
 * Useful for logging and debugging.
 *
 * @return A heap-allocated string containing formatted memory usage info.
 *         Caller must free it using kfree().
 */
KAPI char* get_memory_usage_str();