#pragma once

#include "defines.h"

/**
 * @file event.h
 * @brief Event system for inter-component communication.
 *
 * This module provides a generic event system that allows components to:
 * - Register listeners for specific event codes
 * - Fire events with optional context data
 * - Unregister listeners when no longer needed
 *
 * Events are identified by a 16-bit code, and can carry up to 128 bytes of arbitrary data
 * via the `event_context` union. System-defined event codes use values up to `0xFF`;
 * applications should define their own codes starting from `0x100` or higher.
 */

/**
 * @brief Context structure used to pass event-specific data.
 *
 * This structure contains a union of various primitive types,
 * allowing event handlers to interpret data flexibly based on the event code.
 * Total size is guaranteed to be 128 bytes.
 */
typedef struct event_context {
    /**
     * @brief Union of primitive data types for passing event information. Max 128 bytes
     *
     * Use fields appropriately based on the event code.
     * For example:
     * - Use `.u16[0]` for key/button codes
     * - Use `.u16[0]` and `.u16[1]` for mouse coordinates
     * - Use `.u8[0]` for mouse wheel delta
     * - Use `.f32[4]` for vector-like data
     */
    union {
        // Signed 64-bit integers
        i64 i64[2];
        // Unsigned 64-bit integers
        u64 u64[2];
        // 64-bit floating-point values 
        f64 f64[2];

        // Signed 32-bit integers
        i32 i32[4];
        // Unsigned 32-bit integers
        u32 u32[4];
        // 32-bit floating-point values
        f32 f32[4];


        // Signed 16-bit integers
        i16 i16[8];
        // Unsigned 16-bit integers
        u16 u16[8];  

        // Signed 8-bit integers
        i8 i8[16];
        // Unsigned 8-bit integers
        u8 u8[16];

        // Character or string-like data (e.g., UTF-8 strings)
        char c[16];
    } data;
} event_context;

/**
 * @brief Event handler callback function type.
 *
 * Called when an event matching the registered code is fired.
 *
 * @param code The event code.
 * @param sender A pointer to the object that triggered the event.
 * @param listener_inst A pointer to the listener instance (optional).
 * @param data The event context containing additional data.
 * @return TRUE if the event was handled; FALSE to allow further propagation.
 */
typedef b8 (*PFN_on_event)(u16 code, void* sender, void* listener_inst, event_context data);

/**
 * @brief Initializes the event system.
 *
 * Must be called before any other event functions are used.
 * Sets up internal structures for managing event listeners.
 *
 * @return TRUE if initialization succeeded; FALSE otherwise.
 */
b8 event_initialize();

/**
 * @brief Shuts down the event system.
 *
 * Cleans up internal state, removes all registered listeners,
 * and prepares for safe termination.
 */
void event_shutdown();

/**
 * @brief Registers a listener for a specific event code.
 *
 * Prevents duplicate registrations of the same listener/callback pair.
 * Multiple different callbacks for the same code are allowed.
 *
 * @param code The event code to listen for.
 * @param listener A pointer to a listener instance. Can be NULL.
 * @param on_event The callback function to invoke when the event fires.
 * @return TRUE if successfully registered; FALSE if already registered or invalid parameters.
 */
KAPI b8 event_register(u16 code, void* listener, PFN_on_event on_event);

/**
 * @brief Unregisters a listener for a specific event code.
 *
 * If no matching registration exists, this function returns FALSE.
 *
 * @param code The event code to stop listening for.
 * @param listener A pointer to a listener instance. Can be NULL.
 * @param on_event The callback function to unregister.
 * @return TRUE if successfully unregistered; FALSE if not found.
 */
KAPI b8 event_unregister(u16 code, void* listener, PFN_on_event on_event);

/**
 * @brief Fires an event to all registered listeners for the given code.
 *
 * If any handler returns TRUE, the event is considered handled and
 * will not be passed to remaining listeners.
 *
 * @param code The event code to fire.
 * @param sender A pointer to the object that triggered the event. Can be NULL.
 * @param context The event context containing additional data.
 * @return TRUE if handled by at least one listener; FALSE otherwise.
 */
KAPI b8 event_fire(u16 code, void* sender, event_context context);

/**
 * @brief Built-in system event codes.
 *
 * These are reserved for internal engine use.
 * Applications should use custom codes above `MAX_EVENT_CODE`.
 */
typedef enum system_event_code {
    ///< Shuts the application down on the next frame
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    // Keyboard events
    /**
     * @brief Fired when a keyboard key is pressed.
     *
     * Usage:
     * ```c
     * u16 key_code = context.data.u16[0];
     * ```
     */
    EVENT_CODE_KEY_PRESSED = 0x02,

    /**
     * @brief Fired when a keyboard key is released.
     *
     * Usage:
     * ```c
     * u16 key_code = context.data.u16[0];
     * ```
     */
    EVENT_CODE_KEY_RELEASED = 0x03,

    // Mouse button events
    /**
     * @brief Fired when a mouse button is pressed.
     *
     * Usage:
     * ```c
     * u16 button = context.data.u16[0];
     * ```
     */
    EVENT_CODE_BUTTON_PRESSED = 0x04,

    /**
     * @brief Fired when a mouse button is released.
     *
     * Usage:
     * ```c
     * u16 button = context.data.u16[0];
     * ```
     */
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    // Mouse movement
    /**
     * @brief Fired when the mouse is moved.
     *
     * Usage:
     * ```c
     * u16 x = context.data.u16[0];
     * u16 y = context.data.u16[1];
     * ```
     */
    EVENT_CODE_MOUSE_MOVED = 0x06,

    /**
     * @brief Fired when the mouse wheel is scrolled.
     *
     * Usage:
     * ```c
     * u8 z_delta = context.data.u8[0];
     * ```
     */
    EVENT_CODE_MOUSE_WHEEL = 0x07,

    // Windowing
    /**
     * @brief Fired when the window is resized or resolution changes.
     *
     * Usage:
     * ```c
     * u16 width = context.data.u16[0];
     * u16 height = context.data.u16[1];
     * ```
     */
    EVENT_CODE_RESIZED = 0x08,

    /**
     * @brief Maximum valid system event code.
     *
     * Reserved for internal use — applications should define custom codes beyond this.
     */
    MAX_EVENT_CODE = 0xFF
} system_event_code;