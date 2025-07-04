#include "kmemory.h"

#include "core/logger.h"
#include "platform/platform.h"

// TODO: Add custom string lib
#include <string.h>
#include <stdio.h>

/**
 * @file kmemory.c
 * @brief Implementation of the tagged memory allocation system.
 *
 * This module implements a memory tracking system that allows allocations to be categorized using tags
 * (e.g., MEMORY_TAG_RENDERER, MEMORY_TAG_GAME). It provides:
 * - Memory usage statistics per tag
 * - Allocation and deallocation wrappers with tracking
 * - Helper functions like `kzero_memory()`, `kcopy_memory()`, and `kset_memory()`
 * - A utility function to generate formatted memory usage strings for debugging
 *
 * The system wraps platform-specific memory functions (`platform_allocate`, `platform_free`)
 * to ensure consistent behavior and logging.
 *
 * Usage:
 * Replace raw memory operations with `kallocate()` / `kfree()` to enable tracking.
 */

/**
 * @brief Tracks memory usage statistics.
 *
 * Contains:
 * - Total allocated bytes
 * - Memory usage categorized by allocation tag
 */
struct memory_stats {
    /**
     * @brief Total memory currently allocated in bytes.
     */
    u64 total_allocated;

    /**
     * @brief Array tracking allocations per memory tag type.
     */
    u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

/**
 * @brief Human-readable strings for each memory tag.
 *
 * Used when printing memory usage stats to provide meaningful labels.
 */
static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN      ",
    "ARRAY        ",
    "LINEAR_ALLOC ",
    "DARRAY       ",
    "DICT         ",
    "RING_QUEUE   ",
    "BST          ",
    "STRING       ",
    "APPLICATION  ",
    "JOB          ",
    "TEXTURE      ",
    "MAT_INST     ",
    "RENDERER     ",
    "GAME         ",
    "TRANSFORM    ",
    "ENTITY       ",
    "ENTITY_NODE  ",
    "SCENE        "};

/**
 * @struct memory_system_state
 *
 * @brief Represents the state of the memory tracking system.
 */
typedef struct memory_system_state {
    struct memory_stats stats; /**< Memory usage statistics */
    u64 alloc_count;           /**< Total number of allocations made */
} memory_system_state;

// Global instance of memory tracking state
static memory_system_state* state_ptr;

/**
 * @brief Initializes the memory tracking system.
 *
 * Resets all counters to zero.
 */
void initialize_memory(u64* memory_requirement, void* state) {
    *memory_requirement = sizeof(memory_system_state);

    if (state == 0) {
        return;
    }

    state_ptr = state;
    state_ptr->alloc_count = 0;
    platform_zero_memory(&state_ptr->stats, sizeof(state_ptr->stats));
}

/**
 * @brief Shuts down the memory tracking system.
 *
 * Currently does nothing but can be extended to log final usage or detect leaks.
 */
void shutdown_memory(void* state) {
    // Optional TO-DO: Log final memory usage

    state_ptr = 0;
}

/**
 * @brief Allocates memory with a given size and tag.
 *
 * Adds the allocation to internal statistics and returns a pointer to the block.
 *
 * @param size The number of bytes to allocate.
 * @param tag A memory_tag used to categorize this allocation.
 * @return A pointer to the allocated memory block.
 */
void* kallocate(u64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        KWARN("kallocate called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
    }

    if (state_ptr) {
        state_ptr->stats.total_allocated += size;
        state_ptr->stats.tagged_allocations[tag] += size;
        state_ptr->alloc_count++;
    }

    // TODO: Memory alignment
    void* block = platform_allocate(size, False);
    platform_zero_memory(block, size);

    return block;
}

/**
 * @brief Frees a previously allocated memory block.
 *
 * Subtracts the deallocated size from internal statistics.
 *
 * @param block Pointer to the memory block to free.
 * @param size Size of the block in bytes (must match original allocation).
 * @param tag Tag used when originally allocating the block.
 */
void kfree(void* block, u64 size, memory_tag tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        KWARN("kfree called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
    }

    if (state_ptr) {
        state_ptr->stats.total_allocated -= size;
        state_ptr->stats.tagged_allocations[tag] -= size;
    }

    // TODO: Memory alignment
    platform_free(block, False);
}

/**
 * @brief Fills the provided memory block with zeros.
 *
 * @param block Pointer to the memory block.
 * @param size Number of bytes to zero out.
 * @return Pointer to the zeroed memory block.
 */
void* kzero_memory(void* block, u64 size) {
    return platform_zero_memory(block, size);
}

/**
 * @brief Copies data from one memory block to another.
 *
 * @param dest Destination memory block.
 * @param source Source memory block.
 * @param size Number of bytes to copy.
 * @return Pointer to the destination memory block.
 */
void* kcopy_memory(void* dest, const void* source, u64 size) {
    return platform_copy_memory(dest, source, size);
}

/**
 * @brief Sets every byte in a memory block to a specific value.
 *
 * @param dest Destination memory block.
 * @param value Byte value to fill the block with.
 * @param size Number of bytes to fill.
 * @return Pointer to the filled memory block.
 */
void* kset_memory(void* dest, i32 value, u64 size) {
    return platform_set_memory(dest, value, size);
}

/**
 * @brief Generates a formatted string showing current memory usage per tag.
 *
 * Useful for logging and debugging. Automatically scales values to KB/MB/GB.
 *
 * @return A heap-allocated string containing formatted memory usage info.
 *         Caller must free it using kfree().
 */
char* get_memory_usage_str() {
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buffer[8000] = "System memory use (tagged):\n";
    u64 offset = strlen(buffer);

    for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i) {
        char unit[4] = "XiB";

        float amount = 1.0f;

        if (state_ptr->stats.tagged_allocations[i] >= gib) {
            unit[0] = 'G';
            amount = state_ptr->stats.tagged_allocations[i] / (float)gib;
        } else if (state_ptr->stats.tagged_allocations[i] >= mib) {
            unit[0] = 'M';
            amount = state_ptr->stats.tagged_allocations[i] / (float)mib;
        } else if (state_ptr->stats.tagged_allocations[i] >= kib) {
            unit[0] = 'K';
            amount = state_ptr->stats.tagged_allocations[i] / (float)kib;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)state_ptr->stats.tagged_allocations[i];
        }

        i32 length = snprintf(buffer + offset, 8000, "  %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }

    // char* out_string = _strdup(buffer); // Windows Compatible
    char* out_string = strdup(buffer);  // Note: May need platform_strdup if _strdup isn't available

    return out_string;
}

u64 get_memory_alloc_count() {
    if (state_ptr) {
        return state_ptr->alloc_count;
    } else {
        return 0;  // No memory tracking initialized
    }
}