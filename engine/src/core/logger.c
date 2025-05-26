#include "logger.h"
#include "platform/platform.h"

// TO-DO: TEMPORARY
#include <stdio.h>   // for printf
#include <string.h>  // for memset
#include <stdarg.h>  // for variable argument lists (va_list)
// #include <unistd.h>  // for isatty() - seeing if terminal supports colour


// Terminal supports colour FALSE by default
static b8 terminal_supports_color = FALSE;

// Initialize the logging system.
// For now it just returns TRUE. Later, you can create/open a log file here.
b8 initialize_logging() {
    // TO-DO: Create log file
    return TRUE;
}

// Clean up the logging system.
// Placeholder for cleanup, like flushing buffers or closing the log file.
void shutdown_logging() {
    // TO-DO: Cleanup logging/write queued
}

// Logs a message at a given level, with formatting like printf.
// log_level is an enum (e.g. LOG_LEVEL_INFO, LOG_LEVEL_ERROR, etc.)
// message is a printf-style format string.
// ... are the variable arguments for formatting.
void log_output(log_level level, const char* message, ...) {
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

    // RISKY Code since we are avoding memory allocation which is slow
    // It technically imposes a 32k character limit on a single log entry, this SHOULD NOT HAPPEN
    const i32 msg_length = 32000;
    // Buffer for the formatted message. Large buffer to avoid malloc.
    char out_message[msg_length];
    memset(out_message, 0, sizeof(out_message));

    // Format original message.
    // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type with a "typedef char* va_list" in some
    // cases, and as a result throws a strange error here. The workaround for now is to just use __builtin_va_list,
    // which is the type GCC/Clang's va_start expects.

    // Handle variable arguments manually using built-in va_list
    __builtin_va_list arg_ptr;

    // Start reading after 'message'
    va_start(arg_ptr, message);

    // Format into buffer
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    // Final buffer with level prefix added
    char out_message2[msg_length];
    sprintf(out_message2, "%s%s%s%s\n", level_colors[level], level_strings[level], out_message, level_reset);

    // Platform Specific Output
    if (is_error) {
        platform_console_write_error(out_message2, level);
    } else {
        platform_console_write(out_message2, level);
    }
}

// Called when an assertion fails to log useful debugging info.
void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}