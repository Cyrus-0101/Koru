#pragma once

#include "defines.h"

/**
 * @file logger.h
 * @brief Public interface for the Koru Engine logging system.
 *
 * This module provides a flexible logging system with support for:
 * - Multiple severity levels (FATAL, ERROR, WARN, INFO, DEBUG, TRACE)
 * - Compile-time log level filtering
 * - Color-coded console output
 * - Assertion failure reporting
 *
 * Usage:
 * Use the provided macros (`KFATAL`, `KERROR`, `KWARN`, etc.) to log messages at different severity levels.
 */



// Logging level configuration
// These can be toggled globally or per build configuration

/**
 * @brief Enables warning-level logging output.
 */
#define LOG_WARN_ENABLED 1

/**
 * @brief Enables info-level logging output.
 */
#define LOG_INFO_ENABLED 1

/**
 * @brief Enables debug-level logging output.
 */
#define LOG_DEBUG_ENABLED 1

/**
 * @brief Enables trace-level logging output.
 */
#define LOG_TRACE_ENABLED 1

// Disable debug and trace logs in release builds
#if KRELEASE == 1
#undef LOG_DEBUG_ENABLED
#undef LOG_TRACE_ENABLED
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

/**
 * @brief Log severity levels used for filtering and categorizing log messages.
 *
 * These levels help control how much detail is logged during execution.
 * The logging system can be configured to show only messages at or above a certain level.
 */
typedef enum log_level {
    /**
     * @brief Fatal errors that cause immediate termination.
     *
     * Should be used for unrecoverable failures (e.g., out-of-memory, failed asset load).
     */
    LOG_LEVEL_FATAL = 0,

    /**
     * @brief Non-fatal errors that indicate something went wrong but can be recovered.
     *
     * Examples: Invalid function parameters, failed state transitions.
     */
    LOG_LEVEL_ERROR = 1,

    /**
     * @brief Warnings about unexpected but recoverable situations.
     *
     * Example: Deprecated function usage, resource not found but fallback exists.
     */
    LOG_LEVEL_WARN = 2,

    /**
     * @brief General information messages useful for monitoring application flow.
     *
     * Example: Startup/shutdown events, major state changes.
     */
    LOG_LEVEL_INFO = 3,

    /**
     * @brief Debugging messages useful during development.
     *
     * Example: Function entry/exit, internal state dumps, input/output values.
     */
    LOG_LEVEL_DEBUG = 4,

    /**
     * @brief Fine-grained tracing of program execution.
     *
     * Most verbose level; used for detailed debugging (e.g., every frame update, small logic steps).
     */
    LOG_LEVEL_TRACE = 5,

} log_level;

/**
 * @brief Initializes the logging system.
 *
 * Currently just returns TRUE. In future versions, this may include:
 * - Opening a log file
 * - Initializing mutexes for thread safety
 * - Detecting terminal capabilities
 *
 * @return TRUE if initialization succeeded; FALSE otherwise.
 */
b8 initialize_logging();

/**
 * @brief Shuts down the logging system.
 *
 * Currently just returns. In future versions, this may include:
 * - Flushing buffered logs
 * - Closing log files
 * - Releasing logging resources
 */
void shutdown_logging();

/**
 * @brief Logs a formatted message at the specified log level.
 *
 * Supports standard formatting like `printf()` and includes:
 * - Level prefixing (e.g., "[INFO]: ")
 * - Color coding based on severity (if supported by terminal)
 * - Platform-specific output routing
 *
 * @param level The severity level of the message (e.g., LOG_LEVEL_DEBUG).
 * @param message A format string similar to `printf()`.
 * @param ... Arguments used to replace format specifiers in the message.
 */
KAPI void log_output(log_level level, const char* message, ...);

/**
 * @brief Logs a fatal-level message.
 *
 * Used for unrecoverable errors. Always displayed.
 *
 * Example:
 * ```c
 * KFATAL("Out of memory!");
 * ```
 */
#define KFATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)

#ifndef KERROR
/**
 * @brief Logs an error-level message.
 *
 * Used for recoverable errors.
 *
 * Example:
 * ```c
 * KERROR("Asset failed to load: %s", asset_name);
 * ```
 */
#define KERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#endif

#if LOG_WARN_ENABLED == 1
/**
 * @brief Logs a warning-level message.
 *
 * Used for non-critical issues.
 *
 * Example:
 * ```c
 * KWARN("Deprecated function called");
 * ```
 */
#define KWARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
// No-op version when warnings are disabled
#define KWARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
/**
 * @brief Logs an info-level message.
 *
 * Used for general status updates.
 *
 * Example:
 * ```c
 * KINFO("Engine started successfully.");
 * ```
 */
#define KINFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
// No-op version when info logs are disabled
#define KINFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
/**
 * @brief Logs a debug-level message.
 *
 * Used for developer diagnostics.
 *
 * Example:
 * ```c
 * KDEBUG("Function called with value: %d", x);
 * ```
 */
#define KDEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
// No-op version when debug logs are disabled
#define KDEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
/**
 * @brief Logs a trace-level message.
 *
 * Most verbose level; used for detailed debugging.
 *
 * Example:
 * ```c
 * KTRACE("Frame delta time: %f", dt);
 * ```
 */
#define KTRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
// No-op version when trace logs are disabled
#define KTRACE(message, ...)
#endif