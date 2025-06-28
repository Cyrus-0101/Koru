#pragma once

#include "defines.h"

/**
 * @file linear_allocator.h
 *
 * @brief Linear memory allocator for efficient allocation of fixed-size blocks.
 *
 * This allocator is designed for scenarios where memory is allocated in a linear fashion,
 * such as for temporary buffers or scratch memory. It allows for fast allocation and
 * deallocation of memory blocks without the overhead of managing free lists or complex
 * data structures.
 */

/**
 * @struct linear_allocator
 *
 * @brief Represents a linear memory allocator.
 *
 * Contains metadata about the total size of the allocator, the amount of memory currently allocated,
 */
typedef struct linear_allocator {
    u64 total_size;  // Total size of the allocator
    u64 allocated;   // Total size currently allocated
    void* memory;    // Pointer to the memory block used for allocation
    b8 owns_memory;  // Whether this allocator owns the memory block
} linear_allocator;

/**
 * @brief Creates a linear allocator with the specified total size and memory block.
 * This function initializes the linear allocator with a given size and memory block.
 *
 * @param total_size The total size of the allocator in bytes.
 * @param memory Pointer to the memory block to use for allocation.
 * @param out_allocator Pointer to the linear_allocator structure to initialize.
 * @return void
 */
KAPI void linear_allocator_create(u64 total_size, void* memory, linear_allocator* out_allocator);

/**
 * @brief Destroys the linear allocator.
 *
 * This function cleans up the linear allocator, freeing any resources it owns.
 *
 * @param out_allocator Pointer to the linear_allocator structure to destroy.
 * @return void
 */
KAPI void linear_allocator_destroy(linear_allocator* out_allocator);

/**
 * @brief Allocates a block of memory from the linear allocator.
 *
 * This function allocates a block of memory of the specified size from the linear allocator.
 *
 * @param allocator Pointer to the linear_allocator structure to allocate from.
 * @param size The size of the memory block to allocate in bytes.
 * @return Pointer to the allocated memory block, or NULL if allocation fails.
 */
KAPI void* linear_allocator_allocate(linear_allocator* allocator, u64 size);

/**
 * @brief Frees all allocated memory in the linear allocator.
 *
 * This function resets the allocator, freeing all memory blocks that were allocated.
 * It does not free the memory block itself, only resets the allocation state.
 *
 * @param allocator Pointer to the linear_allocator structure to free.
 * @return void
 */
KAPI void linear_allocator_free_all(linear_allocator* allocator);
