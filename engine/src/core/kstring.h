#pragma once

#include "defines.h"
#include "math/math_types.h"

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

/**
 * @brief Compares two null-terminated strings for equality (case-insensitive).
 *
 * This function performs a character-by-character comparison of two strings, ignoring case differences.
 *
 * @param str0 First string to compare.
 * @param str1 Second string to compare.
 * @return True if both strings are identical (case-insensitive); False otherwise.
 */
KAPI b8 strings_equali(const char* str0, const char* str1);

/**
 * @brief Performs string formatting against the given format string and parameters.
 * NOTE: that this performs a dynamic allocation and should be freed by the caller.
 *
 * @param format The format string to use for the operation
 * @param ... The format arguments.
 * @returns The newly-formatted string (dynamically allocated).
 */
KAPI i32 string_format(char* dest, const char* format, ...);

/**
 * @brief Performs variadic string formatting against the given format string and va_list.
 * NOTE: that this performs a dynamic allocation and should be freed by the caller.
 *
 * @param format The string to be formatted.
 * @param va_list The variadic argument list.
 * @returns The newly-formatted string (dynamically allocated).
 */
KAPI i32 string_format_v(char* dest, const char* format, void* va_list);

/**
 * @brief Copies the source string to the destination string, including the null terminator.
 *
 * @param dest The destination string buffer. Must be large enough to hold the source string.
 * @param source The source null-terminated string to copy.
 * @return A pointer to the destination string.
 */
KAPI char* string_copy(char* dest, const char* source);

/**
 * @brief Copies up to length characters from the source string to the destination string.
 * If the source string is shorter than length, the destination is null-terminated.
 *
 * @param dest The destination string buffer. Must be large enough to hold length characters plus a null terminator.
 * @param source The source null-terminated string to copy from.
 * @param length The maximum number of characters to copy from the source string.
 * @return A pointer to the destination string.
 */
KAPI char* string_ncopy(char* dest, const char* source, i64 length);

/**
 * @brief Trims leading and trailing whitespace from the given string in place.
 *
 * @param str The string to trim. Must be mutable.
 * @return A pointer to the trimmed string (which may be the same as the input pointer).
 */
KAPI char* string_trim(char* str);

/**
 * @brief Extracts a substring from the source string, starting at the given index and of the given length.
 *
 * @param dest The destination buffer to write the substring to. Must be large enough to hold length characters plus a null terminator.
 * @param source The source null-terminated string to extract from.
 * @param start The starting index in the source string (0-based).
 * @param length The number of characters to extract.
 */
KAPI void string_mid(char* dest, const char* source, i32 start, i32 length);

/**
 * @brief Returns the index of the first occurance of c in str; otherwise -1.
 *
 * @param str The string to be scanned.
 * @param c The character to search for.
 * @return The index of the first occurance of c; otherwise -1 if not found.
 */
KAPI i32 string_index_of(char* str, char c);

/**
 * @brief Attempts to parse a vector from the provided string.
 *
 * @param str The string to parse from. Should be space-delimited. (i.e. "1.0 2.0 3.0 4.0")
 * @param out_vector A pointer to the vector to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_vec4(char* str, vec4* out_vector);

/**
 * @brief Attempts to parse a vector from the provided string.
 *
 * @param str The string to parse from. Should be space-delimited. (i.e. "1.0 2.0 3.0")
 * @param out_vector A pointer to the vector to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_vec3(char* str, vec3* out_vector);

/**
 * @brief Attempts to parse a vector from the provided string.
 *
 * @param str The string to parse from. Should be space-delimited. (i.e. "1.0 2.0")
 * @param out_vector A pointer to the vector to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_vec2(char* str, vec2* out_vector);

/**
 * @brief Attempts to parse a 32-bit floating-point number from the provided string.
 *
 * @param str The string to parse from. Should *not* be postfixed with 'f'.
 * @param f A pointer to the float to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_f32(char* str, f32* f);

/**
 * @brief Attempts to parse a 64-bit floating-point number from the provided string.
 *
 * @param str The string to parse from.
 * @param f A pointer to the float to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_f64(char* str, f64* f);

/**
 * @brief Attempts to parse an 8-bit signed integer from the provided string.
 *
 * @param str The string to parse from.
 * @param i A pointer to the int to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_i8(char* str, i8* i);

/**
 * @brief Attempts to parse a 16-bit signed integer from the provided string.
 *
 * @param str The string to parse from.
 * @param i A pointer to the int to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_i16(char* str, i16* i);

/**
 * @brief Attempts to parse a 32-bit signed integer from the provided string.
 *
 * @param str The string to parse from.
 * @param i A pointer to the int to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_i32(char* str, i32* i);

/**
 * @brief Attempts to parse a 64-bit signed integer from the provided string.
 *
 * @param str The string to parse from.
 * @param i A pointer to the int to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_i64(char* str, i64* i);

/**
 * @brief Attempts to parse an 8-bit unsigned integer from the provided string.
 *
 * @param str The string to parse from.
 * @param u A pointer to the int to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_u8(char* str, u8* u);

/**
 * @brief Attempts to parse a 16-bit unsigned integer from the provided string.
 *
 * @param str The string to parse from.
 * @param u A pointer to the int to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_u16(char* str, u16* u);

/**
 * @brief Attempts to parse a 32-bit unsigned integer from the provided string.
 *
 * @param str The string to parse from.
 * @param u A pointer to the int to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_u32(char* str, u32* u);

/**
 * @brief Attempts to parse a 64-bit unsigned integer from the provided string.
 *
 * @param str The string to parse from.
 * @param u A pointer to the int to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_u64(char* str, u64* u);

/**
 * @brief Attempts to parse a boolean from the provided string.
 * "true" or "1" are considered true; anything else is false.
 *
 * @param str The string to parse from. "true" or "1" are considered true; anything else is false.
 * @param b A pointer to the boolean to write to.
 * @return True if parsed successfully; otherwise false.
 */
KAPI b8 string_to_bool(char* str, b8* b);

/**
 *
 * @brief Empties the given string by setting the first character to 0.
 *
 * @param str The string to empty.
 * @return A pointer to the emptied string.
 */
KAPI char* string_empty(char* str);