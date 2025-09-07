#include "core/kstring.h"
#include "core/kmemory.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef _MSC_VER
#include <strings.h>  // for strcasecmp on GCC/Clang
#endif

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

b8 strings_equali(const char* str0, const char* str1) {
#if defined(__GNUC__)
    return strcasecmp(str0, str1) == 0;
#elif defined(_MSC_VER)
    return _stricmp(str0, str1) == 0;
#endif
}

i32 string_format(char* dest, const char* format, ...) {
    if (dest) {
        __builtin_va_list arg_ptr;
        va_start(arg_ptr, format);
        i32 written = string_format_v(dest, format, arg_ptr);
        va_end(arg_ptr);
        return written;
    }
    return -1;
}

i32 string_format_v(char* dest, const char* format, void* va_listp) {
    if (dest) {
        // Big, but can fit on the stack.
        char buffer[32000];
        i32 written = vsnprintf(buffer, 32000, format, va_listp);
        buffer[written] = 0;
        kcopy_memory(dest, buffer, written + 1);

        return written;
    }
    return -1;
}