#include "containers/darray.h"

#include "core/kmemory.h"
#include "core/logger.h"

/**
 * @file darray.c
 * @brief Implementation of a dynamic array container.
 *
 * This module implements a generic dynamic array (darray) system that supports:
 * - Dynamic resizing based on usage
 * - Insertion and removal at arbitrary indices
 * - Type-safe macro wrappers for use in C code
 * - Memory tracking via MEMORY_TAG_DARRAY
 *
 * The internal layout consists of:
 * - A header block containing metadata: capacity, length, stride
 * - A contiguous block of elements stored after the header
 *
 * Usage:
 * This file should not be used directly. Instead, use the macro helpers defined in `darray.h`.
 */

void* _darray_create(u64 length, u64 stride) {
    // Header stores metadata: capacity, length, stride
    u64 header_size = DARRAY_FIELD_LENGTH * sizeof(u64);
    // Total space needed for all elements
    u64 array_size = length * stride;
    // Allocate total size: header + elements
    u64* new_array = kallocate(header_size + array_size, MEMORY_TAG_DARRAY);
    
    // Zero out the entire allocation
    kset_memory(new_array, 0, header_size + array_size);

    // Set initial metadata values
    new_array[DARRAY_CAPACITY] = length;
    new_array[DARRAY_LENGTH] = 0;
    new_array[DARRAY_STRIDE] = stride;

    // Return pointer to the start of the elements section (after metadata)
    return (void*)(new_array + DARRAY_FIELD_LENGTH);
}

void _darray_destroy(void* array) {
    // Get pointer to the start of the header
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    
    // Calculate total size to free
    u64 header_size = DARRAY_FIELD_LENGTH * sizeof(u64);
    u64 total_size = header_size + header[DARRAY_CAPACITY] * header[DARRAY_STRIDE];
    
    // Free memory
    kfree(header, total_size, MEMORY_TAG_DARRAY);
}

u64 _darray_field_get(void* array, u64 field) {
    // Access header just before the array data
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    
    return header[field];
}

void _darray_field_set(void* array, u64 field, u64 value) {
    // Access header just before the array data
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    header[field] = value;
}

void* _darray_resize(void* array) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    // Create a new array with double the capacity
    void* temp = _darray_create(
        (DARRAY_RESIZE_FACTOR * darray_capacity(array)),
        stride);

    // Copy existing elements into the new array
    kcopy_memory(temp, array, length * stride);

    // Update length; capacity is automatically doubled
    _darray_field_set(temp, DARRAY_LENGTH, length);

    // Destroy old array
    _darray_destroy(array);

    return temp;
}

void* _darray_push(void* array, const void* value_ptr) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    // Resize if full
    if (length >= darray_capacity(array)) {
        array = _darray_resize(array);
    }

    // Compute destination address
    u64 addr = (u64)array;
    addr += (length * stride);

    // Copy the value into the array
    kcopy_memory((void*)addr, value_ptr, stride);

    // Update array length
    _darray_field_set(array, DARRAY_LENGTH, length + 1);

    return array;
}

void _darray_pop(void* array, void* dest) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    // Compute source address
    u64 addr = (u64)array;
    addr += ((length - 1) * stride);

    // Copy the last element to the destination
    kcopy_memory(dest, (void*)addr, stride);

    // Reduce the length
    _darray_field_set(array, DARRAY_LENGTH, length - 1);
}

void* _darray_pop_at(void* array, u64 index, void* dest) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    if (index >= length) {
        KERROR("Index outside the bounds of this array! Length: %lu, index: %lu", length, index);
        return array;
    }

    // Compute address of the element to remove
    u64 addr = (u64)array;
    kcopy_memory(dest, (void*)(addr + (index * stride)), stride);

    // Shift remaining elements forward if not removing the last item
    if (index != length - 1) {
        kcopy_memory(
            (void*)(addr + (index * stride)),
            (void*)(addr + ((index + 1) * stride)),
            stride * (length - index));
    }

    // Update array length
    _darray_field_set(array, DARRAY_LENGTH, length - 1);

    return array;
}


void* _darray_insert_at(void* array, u64 index, void* value_ptr) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    if (index >= length) {
        KERROR("Index outside the bounds of this array! Length: %lu, index: %lu", length, index);
        return array;
    }

    // Resize if full
    if (length >= darray_capacity(array)) {
        array = _darray_resize(array);
    }

    u64 addr = (u64)array;

    // Make room for the new element if not inserting at the end
    if (index < length) {
        kcopy_memory(
            (void*)(addr + ((index + 1) * stride)),
            (void*)(addr + (index * stride)),
            stride * (length - index));
    }

    // Insert the new element
    kcopy_memory((void*)(addr + (index * stride)), value_ptr, stride);

    // Update array length
    _darray_field_set(array, DARRAY_LENGTH, length + 1);

    return array;
}