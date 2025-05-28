#pragma once

#include "defines.h"

/**
 * @file darray.h
 * @brief Dynamic array implementation with automatic resizing.
 *
 * This module provides a flexible dynamic array (darray) system that can store any data type.
 * It supports:
 * - Dynamic resizing based on capacity and growth factor
 * - Insertion and removal at arbitrary indices
 * - Compile-time type safety through macros
 * - Tracking of length, capacity, and stride
 *
 * Usage:
 * Use the macro helpers like `darray_create(type)` and `darray_push()` to work with typed arrays.
 * Never call internal `_darray_*` functions directly.
 */

/**
 * @brief Memory layout of a dynamic array.
 *
 * The dynamic array is implemented as a contiguous block of memory with the following layout:
 *
 * - [0] u64: capacity (number of elements that can be held)
 * - [1] u64: length (number of elements currently stored)
 * - [2] u64: stride (size in bytes of each element)
 * - [3] void*: pointer to the start of the elements array
 */
enum {
    DARRAY_CAPACITY,      ///< Index of the capacity field (max number of elements)
    DARRAY_LENGTH,        ///< Index of the length field (current number of elements)
    DARRAY_STRIDE,        ///< Index of the stride field (size of each element in bytes)
    DARRAY_FIELD_LENGTH   ///< Total number of metadata fields before the first element
};

/**
 * @brief Creates a new dynamic array with a given initial capacity and element size.
 *
 * Internal function. Should not be called directly — use `darray_create(type)` instead.
 *
 * @param length Initial capacity of the array.
 * @param stride Size of each element in bytes.
 * @return A pointer to the newly created dynamic array.
 */
KAPI void* _darray_create(u64 length, u64 stride);

/**
 * @brief Destroys a dynamic array and frees its memory.
 *
 * Internal function. Should not be called directly — use `darray_destroy(array)` instead.
 *
 * @param array Pointer to the dynamic array to destroy.
 */
KAPI void _darray_destroy(void* array);

/**
 * @brief Gets the value of a metadata field from the dynamic array.
 *
 * Internal function. Used by macros like `darray_capacity()` and `darray_length()`.
 *
 * @param array Pointer to the dynamic array.
 * @param field Index of the field to retrieve (e.g., DARRAY_CAPACITY).
 * @return The value of the requested field.
 */
KAPI u64 _darray_field_get(void* array, u64 field);

/**
 * @brief Sets the value of a metadata field in the dynamic array.
 *
 * Internal function. Used by macros like `darray_clear()` and `darray_length_set()`.
 *
 * @param array Pointer to the dynamic array.
 * @param field Index of the field to set.
 * @param value New value for the field.
 */
KAPI void _darray_field_set(void* array, u64 field, u64 value);

/**
 * @brief Resizes the dynamic array when it runs out of space.
 *
 * Doubles the capacity if needed. Internal function used by `darray_push()`.
 *
 * @param array Pointer to the dynamic array.
 * @return A pointer to the resized array (may be different from input).
 */
KAPI void* _darray_resize(void* array);

/**
 * @brief Pushes an element onto the end of the array.
 *
 * If the array is full, it will be resized internally.
 *
 * Internal function. Use `darray_push(array, value)` instead.
 *
 * @param array Pointer to the dynamic array.
 * @param value_ptr Pointer to the value to insert.
 * @return A pointer to the array (possibly reallocated).
 */
KAPI void* _darray_push(void* array, const void* value_ptr);

/**
 * @brief Removes the last element from the array and stores it in `dest`.
 *
 * Internal function. Use `darray_pop(array, &dest)` instead.
 *
 * @param array Pointer to the dynamic array.
 * @param dest Pointer to where the popped element should be copied.
 */
KAPI void _darray_pop(void* array, void* dest);

/**
 * @brief Removes the element at the specified index and copies it to `dest`.
 *
 * Internal function. Use `darray_pop_at(array, index, &dest)` instead.
 *
 * @param array Pointer to the dynamic array.
 * @param index Index of the element to remove.
 * @param dest Pointer to where the removed element should be copied.
 */
KAPI void* _darray_pop_at(void* array, u64 index, void* dest);

/**
 * @brief Inserts an element at the specified index.
 *
 * Internal function. Use `darray_insert_at(array, index, value)` instead.
 *
 * @param array Pointer to the dynamic array.
 * @param index Index where the element should be inserted.
 * @param value_ptr Pointer to the value to insert.
 * @return A pointer to the modified array (possibly reallocated).
 */
KAPI void* _darray_insert_at(void* array, u64 index, void* value_ptr);

// Default configuration
#define DARRAY_DEFAULT_CAPACITY 1  ///< Default initial capacity for new dynamic arrays
#define DARRAY_RESIZE_FACTOR 2     ///< Factor by which the array grows when full

/**
 * @brief Creates a dynamic array with default capacity for a specific type.
 *
 * Example:
 * ```c
 * i32* my_array = darray_create(i32);
 * ```
 */
#define darray_create(type) \
    _darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))

/**
 * @brief Creates a dynamic array with a custom initial capacity.
 *
 * Example:
 * ```c
 * i32* my_array = darray_reserve(i32, 10); // Reserve space for 10 integers
 * ```
 */
#define darray_reserve(type, capacity) \
    _darray_create(capacity, sizeof(type))

/**
 * @brief Destroys a dynamic array and frees its memory.
 *
 * Example:
 * ```c
 * darray_destroy(my_array);
 * ```
 */
#define darray_destroy(array) _darray_destroy(array)

/**
 * @brief Adds an element to the end of the array.
 *
 * Automatically resizes the array if necessary.
 *
 * Example:
 * ```c
 * darray_push(my_array, 42);
 * ```
 */
#define darray_push(array, value)           \
    {                                       \
        typeof(value) temp = value;         \
        array = _darray_push(array, &temp); \
    }

/**
 * @brief Removes the last element from the array and copies it to the destination.
 *
 * Example:
 * ```c
 * i32 value;
 * darray_pop(my_array, &value);
 * ```
 */
#define darray_pop(array, value_ptr) \
    _darray_pop(array, value_ptr)

/**
 * @brief Inserts an element at a specific index.
 *
 * Example:
 * ```c
 * darray_insert_at(my_array, 2, 99); // Insert 99 at index 2
 * ```
 */
#define darray_insert_at(array, index, value)           \
    {                                                   \
        typeof(value) temp = value;                     \
        array = _darray_insert_at(array, index, &temp); \
    }

/**
 * @brief Removes the element at a specific index and copies it to the destination.
 *
 * Example:
 * ```c
 * i32 value;
 * darray_pop_at(my_array, 1, &value); // Remove item at index 1
 * ```
 */
#define darray_pop_at(array, index, value_ptr) \
    _darray_pop_at(array, index, value_ptr)

/**
 * @brief Clears the array without freeing memory.
 *
 * Resets the length to zero.
 *
 * Example:
 * ```c
 * darray_clear(my_array);
 * ```
 */
#define darray_clear(array) \
    _darray_field_set(array, DARRAY_LENGTH, 0)

/**
 * @brief Gets the current capacity (allocated slots) of the array.
 *
 * Example:
 * ```c
 * u64 cap = darray_capacity(my_array);
 * ```
 */
#define darray_capacity(array) \
    _darray_field_get(array, DARRAY_CAPACITY)

/**
 * @brief Gets the current length (number of elements) in the array.
 *
 * Example:
 * ```c
 * u64 len = darray_length(my_array);
 * ```
 */
#define darray_length(array) \
    _darray_field_get(array, DARRAY_LENGTH)

/**
 * @brief Gets the size of each element (in bytes) in the array.
 *
 * Example:
 * ```c
 * u64 stride = darray_stride(my_array);
 * ```
 */
#define darray_stride(array) \
    _darray_field_get(array, DARRAY_STRIDE)

/**
 * @brief Sets the current length of the array.
 *
 * Useful for manually managing array length.
 *
 * Example:
 * ```c
 * darray_length_set(my_array, 5); // Set logical length to 5
 * ```
 */
#define darray_length_set(array, value) \
    _darray_field_set(array, DARRAY_LENGTH, value)