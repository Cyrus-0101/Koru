#pragma once
// Ensures the header is included only once (like #ifndef guard)

#include "defines.h"
// Brings in custom type definitions like i32, b8, KAPI, etc.

// Uncomment this to disable all assertions
#define KASSERTIONS_ENABLED

#ifdef KASSERTIONS_ENABLED

// Platform-specific debug break

// If using Microsoft compiler
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
// For Clang/GCC
#define debugBreak() __builtin_trap()
#endif

// Declares an external function to log assertion failures
KAPI void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);

// Assert without message. Logs and breaks if condition fails
#define KASSERT(expr)                                                \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }

// Assert with custom message. Logs and breaks if condition fails
#define KASSERT_MSG(expr, message)                                        \
    {                                                                     \
        if (expr) {                                                       \
        } else {                                                          \
            report_assertion_failure(#expr, message, __FILE__, __LINE__); \
            debugBreak();                                                 \
        }                                                                 \
    }

// Assert only in debug builds. Does nothing in release builds
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
// If KASSERTIONS_ENABLED is not defined
#define KASSERT(expr)               // Does nothing
#define KASSERT_MSG(expr, message)  // Does nothing
#define KASSERT_DEBUG(expr)         // Does nothing
#endif