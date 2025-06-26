#pragma once

#include "defines.h"

/**
 * @file kstring.h
 * @brief Lightweight string utility library.
 *
 * This module provides basic string operations that are:
 * - Safe(r) than raw C string functions
 * - Integrated with the engine's memory system
 * - Designed for clarity and maintainability
 *
 * These utilities are intended to be used internally within the engine,
 * not exposed to end-user game code (which should use standard C strings).
 */

/**
 * @brief Returns the length of the given null-terminated string.
 *
 * Wraps `strlen()` to provide a consistent interface within the engine.
 *
 * @param str A pointer to a null-terminated string.
 * @return The number of characters in the string (excluding the null terminator).
 */
KAPI u64 string_length(const char* str);

/**
 * @brief Duplicates the given null-terminated string into a new heap-allocated copy.
 *
 * Uses `kallocate()` so the duplicated string can be tracked by the engine's memory system.
 *
 * @param str A pointer to a null-terminated string to duplicate.
 * @return A newly allocated copy of the input string. Caller must free using `kfree()` or `string_free()`.
 */
KAPI char* string_duplicate(const char* str);

/**
 * @brief Compares two null-terminated strings for equality (case-sensitive).
 *
 * This function performs a character-by-character comparison of two strings.
 *
 * @param str0 First string to compare.
 * @param str1 Second string to compare.
 * @return True if both strings are identical; False otherwise.
 */
KAPI b8 strings_equal(const char* str0, const char* str1);