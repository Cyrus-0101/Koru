#pragma once
// Ensures the header is included only once (like #ifndef guard)

#include "defines.h"
// Brings in custom type definitions like i32, b8, KAPI, etc.

/**
 * @file asserts.h
 * @brief Custom assertion macros and debugging utilities for the Koru Engine.
 *
 * This module provides:
 * - Compile-time and debug-time assertions
 * - Platform-agnostic debug break functionality
 * - Integration with the logging system to report failures
 *
 * Assertions can be globally disabled by defining `KASSERTIONS_ENABLED` before including this header.
 * Debug-only assertions are automatically disabled in release builds.
 */

// Uncomment this to disable all assertions
#define KASSERTIONS_ENABLED

#ifdef KASSERTIONS_ENABLED

// Platform-specific debug break

// If using Microsoft Visual C/C++ Compiler
#if _MSC_VER
#include <intrin.h>
/**
 * @brief Triggers a breakpoint on Windows platforms.
 */
#define debugBreak() __debugbreak()
#else
/**
 * @brief Triggers a breakpoint on GCC/Clang platforms.
 */
#define debugBreak() __builtin_trap()
#endif

/**
 * @brief Logs an assertion failure message to the logging system.
 *
 * This function should be implemented in `logger.c` or another source file.
 * It is called internally by the assertion macros when a condition fails.
 *
 * @param expression The stringified failed condition (e.g., `"x != NULL"`).
 * @param message Optional additional context about the assertion.
 * @param file The source file where the assertion occurred.
 * @param line The line number in the source file.
 */
KAPI void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);

/**
 * @brief Basic assertion macro that triggers a debug break if the condition is false.
 *
 * If the condition evaluates to false, it logs the failure and breaks into the debugger.
 *
 * Example:
 * ```c
 * KASSERT(ptr != NULL);  // Breaks if ptr is NULL
 * ```
 */
#define KASSERT(expr)                                                \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }

/**                                                                                          \
 * @brief Assertion macro with a custom message.                                             \
 *                                                                                           \
 * If the condition evaluates to false, it logs the failure along with the provided message, \
 * then breaks into the debugger.                                                            \
 *                                                                                           \
 * Example:                                                                                  \
 * ```c                                                                                      \
 * KASSERT_MSG(value >= 0 && value <= 1, "Value must be normalized");                        \
 * ```                                                                                       \
 */                                                                                          \
#define KASSERT_MSG(expr, message)                                                           \
    {                                                                                        \
        if (expr) {                                                                          \
        } else {                                                                             \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);                    \
            debugBreak();                                                                    \
        }                                                                                    \
    }

/**
 * @brief Debug-only assertion. Enabled only in debug builds.
 *
 * This assertion does nothing in release mode.
 *
 * Example:
 * ```c
 * KASSERT_DEBUG(index < array_length);
 * ```
 */
#ifdef _DEBUG
#define KASSERT_DEBUG(expr)                                          \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }
#else
// Disabled in release mode
#define KASSERT_DEBUG(expr)
#endif

#else
// If assertions are globally disabled

/**                                                                       \
 * @brief No-op version of KASSERT when assertions are globally disabled. \
 */
#define KASSERT(expr)

/**
 * @brief No-op version of KASSERT_MSG when assertions are globally disabled.
 */
#define KASSERT_MSG(expr, message)

/**
 * @brief No-op version of KASSERT_DEBUG when assertions are globally disabled.
 */
#define KASSERT_DEBUG(expr)
#endif