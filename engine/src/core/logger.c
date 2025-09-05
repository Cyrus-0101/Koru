#include "logger.h"
#include "kmemory.h"
#include "kstring.h"
#include "platform/platform.h"
#include "platform/filesystem.h"

// TO-DO: TEMPORARY
#include <stdarg.h>  // for variable argument lists (va_list)
// #include <unistd.h>  // for isatty() - seeing if terminal supports colour

/**
 * @brief Maximum length of a log message.
 */
#define MSG_LENGTH 32000

/**
 * @file logger.c
 * @brief Implementation of the logging system.
 *
 * Implements the logging functionality with color-coded terminal output,
 * message formatting, and integration with platform-specific console writes.
 *
 * Features:
 * - Level-based filtering
 * - ANSI escape codes for color output
 * - Platform abstraction for console output
 * - Assertion failure handler
 */

/**
 * @struct logger_system_state
 *
 * @brief Tracks the state of the logging system.
 */
typedef struct logger_system_state {
    /**
     * @brief Handle to the log file.
     */
    file_handle log_file_handle;
} logger_system_state;

// Pointer to the logger system state.
static logger_system_state* state_ptr;

/**
 * @brief Appends a message to the log file.
 *
 * @param message The message to append.
 */
void append_to_log_file(const char* message) {
    if (state_ptr && state_ptr->log_file_handle.is_valid) {
        // Since the message contains '\n' write it out
        u64 length = string_length(message);
        u64 written = 0;

        if (!filesystem_write(&state_ptr->log_file_handle, length, message, &written)) {
            platform_console_write_error("Error writing to console.log.", LOG_LEVEL_ERROR);
        }
    }
}

// Initialize the logging system.
// For now it just returns True. Later, you can create/open a log file here.
b8 initialize_logging(u64* memory_requirement, void* state) {
    *memory_requirement = sizeof(logger_system_state);

    if (state == 0) {
        return True;  // No state provided, nothing to initialize
    }

    state_ptr = state;

    // Create new/wipe existing log_file, then open it
    if (!filesystem_open("console.log", FILE_MODE_WRITE, False, &state_ptr->log_file_handle)) {
        platform_console_write_error("Failed to open console.log for writing.", LOG_LEVEL_ERROR);
        return False;  // Failed to open log file
    }

    // TO-DO: Create log file
    return True;
}

// Clean up the logging system.
// Placeholder for cleanup, like flushing buffers or closing the log file.
void shutdown_logging(void* state) {
    // TO-DO: Cleanup logging/write queued

    state_ptr = 0;
}

// Logs a message at a given level, with formatting like printf.
// log_level is an enum (e.g. LOG_LEVEL_INFO, LOG_LEVEL_ERROR, etc.)
// message is a printf-style format string.
// ... are the variable arguments for formatting.
void log_output(log_level level, const char* message, ...) {
    /**
     * TO-DO: Move string operations to their own thread
     * The ops are pretty slow, and can slow down the engine during startup/shutdown.
     * Work on threading and offset these ops.
     */
    // ANSI color prefixes
    const char* level_colors[6] = {
        "\033[1;31m",  // FATAL - bold red
        "\033[0;31m",  // ERROR - red
        "\033[0;33m",  // WARN  - yellow
        "\033[0;32m",  // INFO  - green
        "\033[0;36m",  // DEBUG - cyan
        "\033[0;37m"   // TRACE - light gray
    };

    // Resets the terminal color
    const char* level_reset = "\033[0m";

    // Level prefixes to show log severity
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};

    // True if the log level is FATAL or ERROR
    b8 is_error = level < LOG_LEVEL_WARN;

    // RISKY Code since we are avoiding memory allocation which is slow
    // It technically imposes a 32k character limit on a single log entry, this SHOULD NOT HAPPEN
    // Buffer for the formatted message. Large buffer to avoid malloc.
    char out_message[MSG_LENGTH];
    kzero_memory(out_message, sizeof(out_message));

    // Format original message.
    // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type with a "typedef char* va_list" in some
    // cases, and as a result throws a strange error here. The workaround for now is to just use __builtin_va_list,
    // which is the type GCC/Clang's va_start expects.

    // Handle variable arguments manually using built-in va_list
    __builtin_va_list arg_ptr;

    // Start reading after 'message'
    va_start(arg_ptr, message);

    // Format into buffer
    string_format_v(out_message, message, arg_ptr);
    va_end(arg_ptr);

    // Final buffer with level prefix added
    char out_message2[MSG_LENGTH];
    string_format(out_message2, "%s%s%s%s\n", level_colors[level], level_strings[level], out_message, level_reset);

    // Platform Specific Output
    if (is_error) {
        platform_console_write_error(out_message2, level);
    } else {
        platform_console_write(out_message2, level);
    }

    append_to_log_file(out_message2);
}

// Called when an assertion fails to log useful debugging info.
void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}