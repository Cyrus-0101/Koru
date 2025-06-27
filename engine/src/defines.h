#pragma once

/**
 * @file defines.h
 * @brief Core definitions and platform-specific utilities.
 *
 * This module provides:
 * - Common type definitions (i8, u32, b8, etc.)
 * - Platform detection macros (Windows, Linux, etc.)
 * - Compiler-specific attributes and function modifiers
 * - Assert and logging macro helpers
 */

// Unsigned integer types with guaranteed sizes

/** 8-bit unsigned integer (1 byte) */
typedef unsigned char u8;

/** 16-bit unsigned integer (2 bytes) */
typedef unsigned short u16;

/** 32-bit unsigned integer (4 bytes) */
typedef unsigned int u32;

/** 64-bit unsigned integer (8 bytes) */
typedef unsigned long long u64;

// Signed integer types with guaranteed sizes

/** 8-bit signed integer (1 byte) */
typedef signed char i8;

/** 16-bit signed integer (2 bytes) */
typedef signed short i16;

/** 32-bit signed integer (4 bytes) */
typedef signed int i32;

/** 64-bit signed integer (8 bytes) */
typedef signed long long i64;

// Floating-point types

/** 32-bit floating point (4 bytes) */
typedef float f32;

/** 64-bit floating point (8 bytes) */
typedef double f64;

// Boolean types

/** 32-bit boolean (can be True/False) */
typedef int b32;
/** 8-bit boolean (more memory-efficient than b32) */
typedef _Bool b8;

/** @brief A range, typically of memory */
typedef struct krange {
    /** @brief The offset in bytes. */
    u64 offset;
    /** @brief The size in bytes. */
    u64 size;
} krange;

/** @brief A range, typically of memory */
typedef struct range32 {
    /** @brief The offset in bytes. */
    i32 offset;
    /** @brief The size in bytes. */
    i32 size;
} range32;

/**
 * Define STATIC_ASSERT based on compiler support.
 * Uses C11 standard `_Static_assert` if available,
 * otherwise falls back to a custom `static_assert` macro.
 */
#if defined(__clang__) || defined(__GNUC__)
#define STATIC_ASSERT _Static_assert /** C11 standard for static assertions */
#else
#define STATIC_ASSERT static_assert /** Fallback for other compilers */
#endif

/**
 * Ensure that the size of each type matches expectations
 * This helps avoid issues across different platforms and compilers
 */

/** Unsigned types */
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");

/** Unsigned types */
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");

/** Unsigned types */
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");

/** Unsigned types */
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

/** Signed types */
STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");

/** Signed types */
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");

/** Signed types */
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");

/** Signed types */
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");

/** Floating point types */
STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");

/** Floating point types */
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

/** Standard boolean values */
#define True 1
/** Standard boolean values */
#define False 0

/** @brief Represents an invalid 64-bit unsigned ID (max value of u64). */
#define INVALID_ID_U64 18446744073709551615UL

/** @brief Represents an invalid 32-bit unsigned ID (max value of u32). */
#define INVALID_ID 4294967295U

/** @brief Represents an invalid 32-bit unsigned ID (max value of u32). */
#define INVALID_ID_U32 INVALID_ID

/** @brief Represents an invalid 16-bit unsigned ID (max value of u16). */
#define INVALID_ID_U16 65535U

/** @brief Represents an invalid 8-bit unsigned ID (max value of u8). */
#define INVALID_ID_U8 255U

/** @brief Maximum value for a 64-bit unsigned integer. */
#define U64_MAX 18446744073709551615UL

/** @brief Maximum value for a 32-bit unsigned integer. */
#define U32_MAX 4294967295U

/** @brief Maximum value for a 16-bit unsigned integer. */
#define U16_MAX 65535U

/** @brief Maximum value for an 8-bit unsigned integer. */
#define U8_MAX 255U

/** @brief Minimum value for unsigned 64 bit integers (0 for all sizes). */
#define U64_MIN 0UL

/** @brief Minimum value for unsigned 32 bit integers (0 for all sizes). */
#define U32_MIN 0U

/** @brief Minimum value for unsigned 16 bit integers (0 for all sizes). */
#define U16_MIN 0U

/** @brief Minimum value for unsigned 8 bit integers (0 for all sizes). */
#define U8_MIN 0U

/** @brief Maximum value for an 8-bit signed integer. */
#define I8_MAX 127

/** @brief Maximum value for a 16-bit signed integer. */
#define I16_MAX 32767

/** @brief Maximum value for a 32-bit signed integer. */
#define I32_MAX 2147483647

/** @brief Maximum value for a 64-bit signed integer. */
#define I64_MAX 9223372036854775807L

/** @brief Minimum value for an 8-bit signed integer. */
#define I8_MIN (-I8_MAX - 1)

/** @brief Minimum value for a 16-bit signed integer. */
#define I16_MIN (-I16_MAX - 1)

/** @brief Minimum value for a 32-bit signed integer. */
#define I32_MIN (-I32_MAX - 1)

/** @brief Minimum value for a 64-bit signed integer. */
#define I64_MIN (-I64_MAX - 1)

/**
 * Detects the current platform and defines corresponding macros:
 * - KPLATFORM_WINDOWS
 * - KPLATFORM_LINUX
 * - KPLATFORM_ANDROID
 * - KPLATFORM_UNIX
 * - KPLATFORM_POSIX
 * - KPLATFORM_APPLE / KPLATFORM_IOS / KPLATFORM_MAC
 */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define KPLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif

#elif defined(__linux__) || defined(__gnu_linux__)
#define KPLATFORM_LINUX 1
#if defined(__ANDROID__)
#define KPLATFORM_ANDROID 1
#endif

#elif defined(__unix__)
#define KPLATFORM_UNIX 1

#elif defined(_POSIX_VERSION)
#define KPLATFORM_POSIX 1

#elif __APPLE__
#define KPLATFORM_APPLE 1
#include <TargetConditionals.h>

#if TARGET_IPHONE_SIMULATOR
#define KPLATFORM_IOS 1
#define KPLATFORM_IOS_SIMULATOR 1

#elif TARGET_OS_IPHONE
#define KPLATFORM_IOS 1

#elif TARGET_OS_MAC
/**
 * @brief Forces Vulkan to use macOS-specific extensions.
 * @note This should ideally be in the Vulkan renderer, not here.
 */
#define VK_USE_PLATFORM_MACOS_MVK
/**
 * Other kinds of Mac OS
 * No additional defines needed beyond KPLATFORM_APPLE
 */
#else
#error "Unknown Apple platform"
#endif

#else
#error "Unknown platform!"
#endif

/**
 *  =============
 *  DLL EXPORT/IMPORT
 *  =============
 */

/**
 * Usage:
 * - When building the engine as a DLL/shared library, define KEXPORT
 * - When using the engine from an application, do not define KEXPORT
 *
 * MSVC uses `__declspec(dllexport)`
 * GCC or Clang uses `__attribute__((visibility("default")))`
 */

#ifdef KEXPORT
/** Building the library – export symbols */
#ifdef _MSC_VER
#define KAPI __declspec(dllexport) /** Windows MSVC */
#else
#define KAPI __attribute__((visibility("default"))) /** GCC/Clang */
#endif
#else
/**
 * Using the library – import symbols
 * MSVC uses `__declspec(dllimport)`
 * GCC/Clang doesn't need special import
 */
#ifdef _MSC_VER
#define KAPI __declspec(dllimport) /** Windows MSVC */
#else
#define KAPI /** GCC/Clang doesn't need special import */
#endif
#endif

#if _DEBUG
#define KOHI_DEBUG 1   /** @brief Defined as 1 in debug builds, 0 otherwise. */
#define KOHI_RELEASE 0 /** @brief Defined as 1 in release builds, 0 otherwise. */
#else
#define KOHI_RELEASE 1 /** @brief Defined as 1 in release builds, 0 otherwise. */
#define KOHI_DEBUG 0   /** @brief Defined as 1 in debug builds, 0 otherwise. */
#endif

/**
 * Clamps a value between min and max.
 * Useful for input sanitization and safe bounds checking.
 *
 * @param value The value to clamp
 * @param min The lower bound
 * @param max The upper bound
 * @return The clamped value
 */
#define KCLAMP(value, min, max) (value <= min) ? min : (value >= max) ? max \
                                                                      : value;

#ifdef _MSC_VER
/**
 * Inlining directives.
 * Used for performance-critical functions.
 * Forces inlining of a function (MSVC: __forceinline).
 * GCC or Clang: uses static inline by default.
 */
#define KINLINE __forceinline
/**
 * Inlining directives.
 * Used for performance-critical functions.
 * Forces inlining of a function (MSVC: __forceinline).
 * GCC or Clang: uses static inline by default.
 */
#define KNOINLINE __declspec(noinline)
#else
/**
 * Inlining directives.
 * Used for performance-critical functions.
 * Forces inlining of a function (MSVC: __forceinline).
 * GCC or Clang: uses static inline by default.
 */
#define KINLINE static inline
/**
 * Inlining directives.
 * Used for performance-critical functions.
 * Forces inlining of a function (MSVC: __forceinline).
 * GCC or Clang: uses static inline by default.
 */
#define KNOINLINE
#endif

// Deprecation
#if defined(__clang__) || defined(__gcc__)
/** @brief Mark something (i.e. a function) as deprecated. */
#define KDEPRECATED(message) __attribute__((deprecated(message)))
#elif defined(_MSC_VER)
/** @brief Mark something (i.e. a function) as deprecated. */
#define KDEPRECATED(message) __declspec(deprecated(message))
#else
#error "Unsupported compiler - don't know how to define deprecations!"
#endif

/** @brief Gets the number of bytes from amount of gibibytes (GiB) (1024*1024*1024) */
#define GIBIBYTES(amount) ((amount) * 1024ULL * 1024ULL * 1024ULL)

/** @brief Gets the number of bytes from amount of mebibytes (MiB) (1024*1024) */
#define MEBIBYTES(amount) ((amount) * 1024ULL * 1024ULL)

/** @brief Gets the number of bytes from amount of kibibytes (KiB) (1024) */
#define KIBIBYTES(amount) ((amount) * 1024ULL)

/** @brief Gets the number of bytes from amount of gigabytes (GB) (1000*1000*1000) */
#define GIGABYTES(amount) ((amount) * 1000ULL * 1000ULL * 1000ULL)

/** @brief Gets the number of bytes from amount of megabytes (MB) (1000*1000) */
#define MEGABYTES(amount) ((amount) * 1000ULL * 1000ULL)

/** @brief Gets the number of bytes from amount of kilobytes (KB) (1000) */
#define KILOBYTES(amount) ((amount) * 1000ULL)

/**
 * @brief Aligns a value to the nearest multiple of a granularity.
 * Useful for memory alignment (e.g., Vulkan buffers).
 *
 * @param operand The value to align.
 * @param granularity The alignment boundary (must be power of 2).
 * @return The aligned value.
 */
KINLINE u64 get_aligned(u64 operand, u64 granularity) {
    return ((operand + (granularity - 1)) & ~(granularity - 1));
}

/**
 * @brief Aligns both the offset and size of a memory range to a granularity.
 *
 * @param offset The starting offset to align.
 * @param size The size to align.
 * @param granularity The alignment boundary (must be power of 2).
 * @return An aligned krange.
 */
KINLINE krange get_aligned_range(u64 offset, u64 size, u64 granularity) {
    return (krange){get_aligned(offset, granularity), get_aligned(size, granularity)};
}

/** @brief Returns the smaller of two values. */
#define KMIN(x, y) (x < y ? x : y)

/** @brief Returns the larger of two values. */
#define KMAX(x, y) (x > y ? x : y)

/**
 * @brief Checks if a specific flag is enabled in a bitmask.
 *
 * @param flags The bitmask to check.
 * @param flag The flag to test.
 * @return True if the flag is set; otherwise false.
 */
#define FLAG_GET(flags, flag) ((flags & flag) == flag)

/**
 * @brief Sets or clears a flag in a bitmask.
 *
 * @param flags The bitmask to modify (passed by reference).
 * @param flag The flag to set/clear.
 * @param enabled If true, sets the flag; if false, clears it.
 */
#define FLAG_SET(flags, flag, enabled) (flags = (enabled ? (flags | flag) : (flags & ~flag)))