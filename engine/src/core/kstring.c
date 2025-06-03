#include "core/kstring.h"
#include "core/kmemory.h"

#include <string.h>

/**
 * @file kstring.c
 * @brief Implementation of lightweight string utility functions.
 *
 * Implements core string functionality used internally by the engine.
 * Currently wraps standard C library functions but integrates with the engine’s custom memory system.
 *
 * Usage:
 * These functions are primarily used by engine systems that need to:
 * - Copy strings safely
 * - Duplicate strings with engine-aware memory allocation
 * - Interact with memory tracking via MEMORY_TAG_STRING
 */

u64 string_length(const char* str) {
    return strlen(str);
}

char* string_duplicate(const char* str) {
    u64 length = string_length(str);
    
    char* copy = kallocate(length + 1, MEMORY_TAG_STRING);
    
    kcopy_memory(copy, str, length + 1);
    
    return copy;
}

b8 strings_equal(const char* str0, const char* str1) {
    return strcmp(str0, str1) == 0;
}