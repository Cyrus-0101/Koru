#pragma once  // Ensures this header is only included once per compilation unit

#include "defines.h"  // Core definitions: types, asserts, platform detection, etc.

/**
 * @brief Opaque state structure for the platform layer.
 *
 * This struct holds internal platform-specific data that the implementation needs,
 * such as window handles, device contexts, input state, etc.
 *
 * The actual contents are defined privately in platform_*.c files.
 */
typedef struct platform_state {
    /**
     * @brief A pointer to internal platform-specific state.
     *
     * This will point to a platform-specific struct (e.g., Win32PlatformState, LinuxPlatformState)
     * which contains all the low-level details needed by the implementation.
     */
    void* internal_state;
} platform_state;

/**
 * @brief Initializes the platform layer.
 *
 * Sets up:
 * - Window creation
 * - Input devices
 * - Platform-specific memory and timing systems
 *
 * @param plat_state A pointer to an uninitialized platform_state struct.
 * @param application_name The name shown in the window title bar.
 * @param x X position of the window on the screen.
 * @param y Y position of the window on the screen.
 * @param width Width of the window client area.
 * @param height Height of the window client area.
 * @return TRUE if initialization was successful; FALSE otherwise.
 */
KAPI b8 platform_startup(
    platform_state* plat_state,
    const char* application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height);

/**
 * @brief Shuts down the platform layer.
 *
 * Cleans up:
 * - Window resources
 * - Input devices
 * - Any allocated memory or threads
 *
 * @param plat_state A pointer to the initialized platform_state struct.
 */
KAPI void platform_shutdown(platform_state* plat_state);

/**
 * @brief Processes messages/events from the operating system.
 *
 * Handles events like:
 * - Keyboard input
 * - Mouse movement and clicks
 * - Window resizing
 * - Close button click
 *
 * @param plat_state A pointer to the initialized platform_state struct.
 * @return TRUE if the application should continue running; FALSE if quit was requested.
 */
KAPI b8 platform_pump_messages(platform_state* plat_state);

/**
 * @brief Allocates memory from the platform.
 *
 * @param size The number of bytes to allocate.
 * @param aligned Whether the allocation should be aligned for performance (e.g., SIMD).
 * @return A pointer to the allocated memory block.
 */
void* platform_allocate(u64 size, b8 aligned);

/**
 * @brief Frees a previously allocated block of memory.
 *
 * @param block A pointer to the memory block to free.
 * @param aligned Whether the block was allocated with alignment.
 */
void platform_free(void* block, b8 aligned);

/**
 * @brief Fills a block of memory with zeros.
 *
 * @param block A pointer to the memory block.
 * @param size The number of bytes to zero out.
 * @return A pointer to the zeroed memory block.
 */
void* platform_zero_memory(void* block, u64 size);

/**
 * @brief Copies bytes from one memory block to another.
 *
 * @param dest A pointer to the destination memory block.
 * @param source A pointer to the source memory block.
 * @param size The number of bytes to copy.
 * @return A pointer to the destination memory block.
 */
void* platform_copy_memory(void* dest, const void* source, u64 size);

/**
 * @brief Fills a block of memory with a specific byte value.
 *
 * @param dest A pointer to the memory block.
 * @param value The byte value to set.
 * @param size The number of bytes to fill.
 * @return A pointer to the filled memory block.
 */
void* platform_set_memory(void* dest, i32 value, u64 size);

/**
 * @brief Writes a message to the platform console with a given color.
 *
 * Useful for logging and debugging output.
 *
 * @param message The message to write.
 * @param colour A numeric identifier for the color (e.g., red, green, yellow).
 */
void platform_console_write(const char* message, u8 colour);

/**
 * @brief Writes an error message to the platform console with a color.
 *
 * Used specifically for error-level logs.
 *
 * @param message The error message to write.
 * @param colour A numeric identifier for the color.
 */
void platform_console_write_error(const char* message, u8 colour);

/**
 * @brief Gets the current time in seconds since some arbitrary fixed point in the past.
 *
 * Designed to be monotonic (never goes backward), suitable for measuring durations.
 *
 * @return The current time in seconds.
 */
f64 platform_get_absolute_time();

/**
 * @brief Suspends the calling thread for at least the specified number of milliseconds.
 *
 * This is a blocking call and should only be used when giving unused CPU time back to the OS.
 *
 * @param ms The number of milliseconds to sleep.
 */
void platform_sleep(u64 ms);