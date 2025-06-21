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

/** 32-bit boolean (can be TRUE/FALSE) */
typedef int b32;
/** 8-bit boolean (more memory-efficient than b32) */
typedef char b8;

/** Define STATIC_ASSERT based on compiler support */
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
#define TRUE 1
/** Standard boolean values */
#define FALSE 0

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
 */

#ifdef KEXPORT
/** Building the library – export symbols */
#ifdef _MSC_VER
#define KAPI __declspec(dllexport) /** Windows MSVC */
#else
#define KAPI __attribute__((visibility("default"))) /** GCC/Clang */
#endif
#else
/** Using the library – import symbols */
#ifdef _MSC_VER
#define KAPI __declspec(dllimport) /** Windows MSVC */
#else
#define KAPI /** GCC/Clang doesn't need special import */
#endif
#endif

#define KCLAMP(value, min, max) (value <= min) ? min : (value >= max) ? max \
                                                                      : value;

// Inlining
#ifdef _MSC_VER
#define KINLINE __forceinline
#define KNOINLINE __declspec(noinline)
#else
#define KINLINE static inline
#define KNOINLINE
#endif