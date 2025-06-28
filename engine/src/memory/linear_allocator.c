#include "core/kmemory.h"
#include "core/logger.h"
#include "linear_allocator.h"

/**
 *
 * @file linear_allocator.c
 *
 * @brief Implementation of a linear memory allocator.
 *
 * This file implements functions to create, destroy, allocate, and free memory
 */

void linear_allocator_create(u64 total_size, void* memory, linear_allocator* out_allocator) {
    if (out_allocator) {
        out_allocator->total_size = total_size;
        out_allocator->allocated = 0;
        out_allocator->owns_memory = memory == 0;  // if memory is 1, the allocator owns it, if memory is 0, it does not own it

        if (memory) {
            out_allocator->memory = memory;
        } else {
            out_allocator->memory = kallocate(total_size, MEMORY_TAG_LINEAR_ALLOCATOR);
        }
    }
}

void linear_allocator_destroy(linear_allocator* out_allocator) {
    if (out_allocator) {
        out_allocator->allocated = 0;
        if (out_allocator->owns_memory && out_allocator->memory) {
            kfree(out_allocator->memory, out_allocator->total_size, MEMORY_TAG_LINEAR_ALLOCATOR);
        }

        out_allocator->memory = 0;  // Reset memory pointer
        out_allocator->total_size = 0;
        out_allocator->owns_memory = False;  // Reset ownership
    }
    // Note: We don't free the out_allocator itself, as it may be a stack-allocated structure.
    // If it was dynamically allocated, the caller should handle that.
    // If you want to free the out_allocator itself, you can do so by calling kfree(out_allocator, sizeof(linear_allocator), MEMORY_TAG_LINEAR_ALLOCATOR);
    // But this is typically not done, as linear_allocator is often used as a stack-allocated structure.
}

void* linear_allocator_allocate(linear_allocator* allocator, u64 size) {
    if (allocator && allocator->memory) {
        if (allocator->allocated + size > allocator->total_size) {
            // Not enough space left in the allocator
            u64 remaining = allocator->total_size - allocator->allocated;
            KFATAL("linear_allocator_alocate - Out of memory! Requested: %lluB, Available: %lluB", size, remaining);
            return 0;  // Allocation failed
        }

        void* block = ((u8*)allocator->memory) + allocator->allocated;
        allocator->allocated += size;

        return block;
    }

    KFATAL("linear_allocator_allocate, provided allocator not initialized");

    return 0;
}

void linear_allocator_free_all(linear_allocator* allocator) {
    if (allocator && allocator->memory) {
        allocator->allocated = 0;
        kzero_memory(allocator->memory, allocator->total_size);
    }
}
