#include "hashtable.h"

#include "core/kmemory.h"
#include "core/logger.h"

/**
 * @file hashtable.c
 * @brief Implementation of a simple hashtable.
 *
 * This module provides a basic hashtable implementation that supports storing and retrieving
 * both pointer and non-pointer types. It uses a simple hashing function to map string keys to
 * indices in a fixed-size array.
 *
 * Usage:
 * - Create a hashtable with `hashtable_create()`, providing element size, count, memory block, and type info.
 * - Use `hashtable_set()` and `hashtable_get()` for non-pointer types.
 * - Use `hashtable_set_ptr()` and `hashtable_get_ptr()` for pointer types.
 * - Destroy the hashtable with `hashtable_destroy()`.
 *
 * Note:
 * - The hashtable does not handle collisions; ensure unique keys or increase element count.
 * - Memory management for pointer types is the responsibility of the caller.
 */

/**
 * @brief Generates a hash value for a given string.
 *
 * Uses a simple hashing algorithm to compute a hash based on character values and a prime multiplier.
 * The resulting hash is then modded by the element_count to ensure it fits within the table size.
 *
 * Simple hash function for strings, based on [djb2 by Dan Bernstein](https://theartincode.stanis.me/008-djb2/).
 *
 * @param name The input string to hash.
 * @param element_count The size of the hashtable (number of elements).
 * @return A hash value in the range [0, element_count - 1].
 */
u64 hash_name(const char* name, u32 element_count) {
    // A multipler to use when generating a hash. Prime to hopefully avoid collisions.
    static const u64 multiplier = 97;

    unsigned const char* us;
    u64 hash = 0;

    for (us = (unsigned const char*)name; *us; us++) {
        hash = hash * multiplier + *us;
    }

    // Mod it against the size of the table.
    hash %= element_count;

    return hash;
}

void hashtable_create(u64 element_size, u32 element_count, void* memory, b8 is_pointer_type, hashtable* out_hashtable) {
    // Confirm if memory exists and is large enough.
    if (!memory || !out_hashtable) {
        KERROR("hashtable_create failed! Pointer to memory and out_hashtable are required.");
        return;
    }

    // Confirm element size and count are valid.
    if (!element_count || !element_size) {
        KERROR("element_size and element_count must be a positive non-zero value.");
        return;
    }

    // TODO: Might want to require an allocator and allocate this memory instead.
    // Cuurently allocated externally.
    out_hashtable->memory = memory;
    out_hashtable->element_count = element_count;
    out_hashtable->element_size = element_size;
    out_hashtable->is_pointer_type = is_pointer_type;
    kzero_memory(out_hashtable->memory, element_size * element_count);
}

void hashtable_destroy(hashtable* table) {
    if (table) {
        // TODO: If using allocator above, free memory here.
        kzero_memory(table, sizeof(hashtable));
    }
}

b8 hashtable_set(hashtable* table, const char* name, void* value) {
    // Validate input.
    if (!table || !name || !value) {
        KERROR("hashtable_set requires table, name and value to exist.");
        return False;
    }

    // Ensure this is not a pointer type table.
    if (table->is_pointer_type) {
        KERROR("hashtable_set should not be used with tables that have pointer types. Use hashtable_set_ptr instead.");
        return False;
    }

    u64 hash = hash_name(name, table->element_count);
    kcopy_memory(table->memory + (table->element_size * hash), value, table->element_size);
    return True;
}

b8 hashtable_set_ptr(hashtable* table, const char* name, void** value) {
    // Validate input, zeroing a pointer is allowed.
    if (!table || !name) {
        KWARN("hashtable_set_ptr requires table and name  to exist.");
        return False;
    }

    // Ensure this is a pointer type table.
    if (!table->is_pointer_type) {
        KERROR("hashtable_set_ptr should not be used with tables that do not have pointer types. Use hashtable_set instead.");
        return False;
    }

    u64 hash = hash_name(name, table->element_count);
    // Cast to a void** and set the pointer (or 0 if value is null).
    ((void**)table->memory)[hash] = value ? *value : 0;
    return True;
}

b8 hashtable_get(hashtable* table, const char* name, void* out_value) {
    // Validate input.
    if (!table || !name || !out_value) {
        KWARN("hashtable_get requires table, name and out_value to exist.");
        return False;
    }

    // Ensure this is not a pointer type table.
    if (table->is_pointer_type) {
        KERROR("hashtable_get should not be used with tables that have pointer types. Use hashtable_set_ptr instead.");
        return False;
    }

    u64 hash = hash_name(name, table->element_count);
    kcopy_memory(out_value, table->memory + (table->element_size * hash), table->element_size);
    return True;
}

b8 hashtable_get_ptr(hashtable* table, const char* name, void** out_value) {
    // Validate input.
    if (!table || !name || !out_value) {
        KWARN("hashtable_get_ptr requires table, name and out_value to exist.");
        return False;
    }

    // Ensure this is a pointer type table.
    if (!table->is_pointer_type) {
        KERROR("hashtable_get_ptr should not be used with tables that do not have pointer types. Use hashtable_get instead.");
        return False;
    }

    u64 hash = hash_name(name, table->element_count);
    // Cast to a void** and get the pointer (or 0 if value is null).
    *out_value = ((void**)table->memory)[hash];
    // Modify return value to indicate if the retrieved pointer is non-null.
    return *out_value != 0;
}

b8 hashtable_fill(hashtable* table, void* value) {
    // Validate input.
    if (!table || !value) {
        KWARN("hashtable_fill requires table and value to exist.");
        return False;
    }

    // Ensure this is not a pointer type table.
    if (table->is_pointer_type) {
        KERROR("hashtable_fill should not be used with tables that have pointer types.");
        return False;
    }

    for (u32 i = 0; i < table->element_count; ++i) {
        kcopy_memory(table->memory + (table->element_size * i), value, table->element_size);
    }

    return True;
}
