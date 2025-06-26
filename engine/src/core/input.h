#pragma once

#include "defines.h"

/**
 * @file input.h
 * @brief Input system interface for handling keyboard and mouse events.
 *
 * This module provides a cross-platform abstraction for:
 * - Keyboard state tracking (pressed/released)
 * - Mouse button and movement detection
 * - Polling and event-based input systems
 *
 * Usage:
 * Call `input_initialize()` at startup, `input_update()` every frame,
 * and use the provided functions to query or process input data.
 */

/**
 * @brief Mouse button codes used in input processing.
 *
 * These represent the primary mouse buttons supported by the system.
 */
typedef enum buttons {
    BUTTON_LEFT,        ///< Left mouse button
    BUTTON_RIGHT,       ///< Right mouse button
    BUTTON_MIDDLE,      ///< Middle mouse button (scroll wheel press)
    BUTTON_MAX_BUTTONS  ///< Total number of defined mouse buttons
} buttons;

/**
 * @brief Helper macro for defining key codes.
 *
 * Used internally to assign numeric values to each key constant.
 *
 * Example:
 * ```c
 * DEFINE_KEY(ENTER, 0x0D);
 * ```
 * Expands to: `KEY_ENTER = 0x0D`
 */
#define DEFINE_KEY(name, code) KEY_##name = code

/**
 * @brief Key codes for standard keyboard keys.
 *
 * Includes alphanumeric, function, modifier, and navigation keys.
 * Values are loosely based on Windows virtual key codes for consistency.
 */
typedef enum keys {
    // Control Keys
    DEFINE_KEY(BACKSPACE, 0x08),  ///< Backspace key
    DEFINE_KEY(ENTER, 0x0D),      ///< Enter/Return key
    DEFINE_KEY(TAB, 0x09),        ///< Tab key
    DEFINE_KEY(SHIFT, 0x10),      ///< Either Shift key
    DEFINE_KEY(CONTROL, 0x11),    ///< Either Ctrl key

    // System Keys
    DEFINE_KEY(PAUSE, 0x13),    ///< Pause/Break key
    DEFINE_KEY(CAPITAL, 0x14),  ///< Caps Lock key

    // Escape & Special Keys
    DEFINE_KEY(ESCAPE, 0x1B),  ///< Escape key

    // IME / Asian Language Keys
    DEFINE_KEY(CONVERT, 0x1C),     ///< IME Convert
    DEFINE_KEY(NONCONVERT, 0x1D),  ///< IME Nonconvert
    DEFINE_KEY(ACCEPT, 0x1E),      ///< IME Accept
    DEFINE_KEY(MODECHANGE, 0x1F),  ///< IME Mode Change

    // Navigation & Editing Keys
    DEFINE_KEY(SPACE, 0x20),     ///< Spacebar
    DEFINE_KEY(PRIOR, 0x21),     ///< Page Up key
    DEFINE_KEY(NEXT, 0x22),      ///< Page Down key
    DEFINE_KEY(END, 0x23),       ///< End key
    DEFINE_KEY(HOME, 0x24),      ///< Home key
    DEFINE_KEY(LEFT, 0x25),      ///< Left arrow key
    DEFINE_KEY(UP, 0x26),        ///< Up arrow key
    DEFINE_KEY(RIGHT, 0x27),     ///< Right arrow key
    DEFINE_KEY(DOWN, 0x28),      ///< Down arrow key
    DEFINE_KEY(SELECT, 0x29),    ///< Select key
    DEFINE_KEY(PRINT, 0x2A),     ///< Print Screen key
    DEFINE_KEY(EXECUTE, 0x2B),   ///< Execute key
    DEFINE_KEY(SNAPSHOT, 0x2C),  ///< Snapshot/PrtScn key
    DEFINE_KEY(INSERT, 0x2D),    ///< Insert key
    DEFINE_KEY(DELETE, 0x2E),    ///< Delete key
    DEFINE_KEY(HELP, 0x2F),      ///< Help key

    // Letter Keys
    DEFINE_KEY(A, 0x41),
    DEFINE_KEY(B, 0x42),
    DEFINE_KEY(C, 0x43),
    DEFINE_KEY(D, 0x44),
    DEFINE_KEY(E, 0x45),
    DEFINE_KEY(F, 0x46),
    DEFINE_KEY(G, 0x47),
    DEFINE_KEY(H, 0x48),
    DEFINE_KEY(I, 0x49),
    DEFINE_KEY(J, 0x4A),
    DEFINE_KEY(K, 0x4B),
    DEFINE_KEY(L, 0x4C),
    DEFINE_KEY(M, 0x4D),
    DEFINE_KEY(N, 0x4E),
    DEFINE_KEY(O, 0x4F),
    DEFINE_KEY(P, 0x50),
    DEFINE_KEY(Q, 0x51),
    DEFINE_KEY(R, 0x52),
    DEFINE_KEY(S, 0x53),
    DEFINE_KEY(T, 0x54),
    DEFINE_KEY(U, 0x55),
    DEFINE_KEY(V, 0x56),
    DEFINE_KEY(W, 0x57),
    DEFINE_KEY(X, 0x58),
    DEFINE_KEY(Y, 0x59),
    DEFINE_KEY(Z, 0x5A),

    // Modifier Keys
    DEFINE_KEY(LWIN, 0x5B),  ///< Left Windows key
    DEFINE_KEY(RWIN, 0x5C),  ///< Right Windows key
    DEFINE_KEY(APPS, 0x5D),  ///< Application (context menu) key

    // Power/Sleep Keys
    DEFINE_KEY(SLEEP, 0x5F),  ///< Sleep key

    // Numpad Keys
    DEFINE_KEY(NUMPAD0, 0x60),
    DEFINE_KEY(NUMPAD1, 0x61),
    DEFINE_KEY(NUMPAD2, 0x62),
    DEFINE_KEY(NUMPAD3, 0x63),
    DEFINE_KEY(NUMPAD4, 0x64),
    DEFINE_KEY(NUMPAD5, 0x65),
    DEFINE_KEY(NUMPAD6, 0x66),
    DEFINE_KEY(NUMPAD7, 0x67),
    DEFINE_KEY(NUMPAD8, 0x68),
    DEFINE_KEY(NUMPAD9, 0x69),
    DEFINE_KEY(MULTIPLY, 0x6A),   ///< Multiply (*)
    DEFINE_KEY(ADD, 0x6B),        ///< Add (+)
    DEFINE_KEY(SEPARATOR, 0x6C),  ///< Separator key (=)
    DEFINE_KEY(SUBTRACT, 0x6D),   ///< Subtract (-)
    DEFINE_KEY(DECIMAL, 0x6E),    ///< Decimal (.)
    DEFINE_KEY(DIVIDE, 0x6F),     ///< Divide (/)

    // Function Keys
    DEFINE_KEY(F1, 0x70),
    DEFINE_KEY(F2, 0x71),
    DEFINE_KEY(F3, 0x72),
    DEFINE_KEY(F4, 0x73),
    DEFINE_KEY(F5, 0x74),
    DEFINE_KEY(F6, 0x75),
    DEFINE_KEY(F7, 0x76),
    DEFINE_KEY(F8, 0x77),
    DEFINE_KEY(F9, 0x78),
    DEFINE_KEY(F10, 0x79),
    DEFINE_KEY(F11, 0x7A),
    DEFINE_KEY(F12, 0x7B),
    DEFINE_KEY(F13, 0x7C),
    DEFINE_KEY(F14, 0x7D),
    DEFINE_KEY(F15, 0x7E),
    DEFINE_KEY(F16, 0x7F),
    DEFINE_KEY(F17, 0x80),
    DEFINE_KEY(F18, 0x81),
    DEFINE_KEY(F19, 0x82),
    DEFINE_KEY(F20, 0x83),
    DEFINE_KEY(F21, 0x84),
    DEFINE_KEY(F22, 0x85),
    DEFINE_KEY(F23, 0x86),
    DEFINE_KEY(F24, 0x87),

    // Lock Keys
    DEFINE_KEY(NUMLOCK, 0x90),  ///< Num Lock key
    DEFINE_KEY(SCROLL, 0x91),   ///< Scroll Lock key

    // Numpad Additional Keys
    DEFINE_KEY(NUMPAD_EQUAL, 0x92),  ///< Numpad Equals key

    // Extended Modifier Keys
    DEFINE_KEY(LSHIFT, 0xA0),    ///< Left Shift key
    DEFINE_KEY(RSHIFT, 0xA1),    ///< Right Shift key
    DEFINE_KEY(LCONTROL, 0xA2),  ///< Left Control key
    DEFINE_KEY(RCONTROL, 0xA3),  ///< Right Control key
    DEFINE_KEY(LMENU, 0xA4),     ///< Left Alt key
    DEFINE_KEY(RMENU, 0xA5),     ///< Right Alt key

    // Punctuation and Symbols
    DEFINE_KEY(SEMICOLON, 0xBA),  ///< Semicolon (;)
    DEFINE_KEY(PLUS, 0xBB),       ///< Plus (+)
    DEFINE_KEY(COMMA, 0xBC),      ///< Comma (,)
    DEFINE_KEY(MINUS, 0xBD),      ///< Minus (-)
    DEFINE_KEY(PERIOD, 0xBE),     ///< Period (.)
    DEFINE_KEY(SLASH, 0xBF),      ///< Slash (/)
    DEFINE_KEY(GRAVE, 0xC0),      ///< Grave accent (`)

    KEYS_MAX_KEYS  ///< Total number of defined keys
} keys;

/**
 * @brief Initializes the input system.
 *
 * Sets up internal state tracking for keyboard and mouse.
 */
void input_initialize();

/**
 * @brief Shuts down the input system.
 *
 * Cleans up any resources used by the input system.
 */
void input_shutdown();

/**
 * @brief Updates the input system state.
 *
 * Should be called once per frame to update current and previous input states.
 *
 * @param delta_time Time since the last update. Unused here but part of update signature pattern.
 */
void input_update(f64 delta_time);

// -------------------------------
// 🎮 Keyboard Input Functions
// -------------------------------

/**
 * @brief Checks if the given key is currently pressed down.
 *
 * @param key The key to check.
 * @return True if the key is currently down; False otherwise.
 */
KAPI b8 input_is_key_down(keys key);

/**
 * @brief Checks if the given key is currently released.
 *
 * @param key The key to check.
 * @return True if the key is currently up; False otherwise.
 */
KAPI b8 input_is_key_up(keys key);

/**
 * @brief Checks if the given key was pressed during the previous frame.
 *
 * Useful for detecting single key presses that occurred between frames.
 *
 * @param key The key to check.
 * @return True if the key was pressed last frame; False otherwise.
 */
KAPI b8 input_was_key_down(keys key);

/**
 * @brief Checks if the given key was released during the previous frame.
 *
 * Useful for detecting key releases that occurred between frames.
 *
 * @param key The key to check.
 * @return True if the key was released last frame; False otherwise.
 */
KAPI b8 input_was_key_up(keys key);

/**
 * @brief Processes a key press or release from the platform layer.
 *
 * This should be called by the platform layer when a key event occurs.
 *
 * @param key The key being processed.
 * @param pressed True if the key is pressed; False if released.
 */
void input_process_key(keys key, b8 pressed);

// -------------------------------
// 🖱️ Mouse Input Functions
// -------------------------------

/**
 * @brief Checks if the given mouse button is currently pressed.
 *
 * @param button The mouse button to check.
 * @return True if the button is down; False otherwise.
 */
KAPI b8 input_is_button_down(buttons button);

/**
 * @brief Checks if the given mouse button is currently released.
 *
 * @param button The mouse button to check.
 * @return True if the button is up; False otherwise.
 */
KAPI b8 input_is_button_up(buttons button);

/**
 * @brief Checks if the given mouse button was pressed in the previous frame.
 *
 * @param button The mouse button to check.
 * @return True if the button was pressed last frame; False otherwise.
 */
KAPI b8 input_was_button_down(buttons button);

/**
 * @brief Checks if the given mouse button was released in the previous frame.
 *
 * @param button The mouse button to check.
 * @return True if the button was released last frame; False otherwise.
 */
KAPI b8 input_was_button_up(buttons button);

/**
 * @brief Gets the current mouse position relative to the window.
 *
 * @param x Pointer to store X coordinate.
 * @param y Pointer to store Y coordinate.
 */
KAPI void input_get_mouse_position(i32* x, i32* y);

/**
 * @brief Gets the mouse position from the previous frame.
 *
 * Useful for computing deltas like cursor speed or drag distance.
 *
 * @param x Pointer to store previous X coordinate.
 * @param y Pointer to store previous Y coordinate.
 */
KAPI void input_get_previous_mouse_position(i32* x, i32* y);

/**
 * @brief Processes a mouse button press or release from the platform layer.
 *
 * Should be called by the platform-specific code when a mouse button changes state.
 *
 * @param button The mouse button involved.
 * @param pressed True if the button is pressed; False if released.
 */
void input_process_button(buttons button, b8 pressed);

/**
 * @brief Processes mouse movement from the platform layer.
 *
 * Called when the mouse moves to update internal position tracking.
 *
 * @param x New X position relative to client area.
 * @param y New Y position relative to client area.
 */
void input_process_mouse_move(i16 x, i16 y);

/**
 * @brief Processes mouse wheel scroll input.
 *
 * @param z_delta Delta value indicating scroll direction and magnitude.
 */
void input_process_mouse_wheel(i8 z_delta);