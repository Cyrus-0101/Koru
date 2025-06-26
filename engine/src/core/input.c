#include "core/input.h"
#include "core/event.h"
#include "core/kmemory.h"
#include "core/logger.h"

/**
 * @file input.c
 * @brief Implementation of the input system for keyboard and mouse events.
 *
 * This module implements a state-based input system that tracks:
 * - Current and previous keyboard states
 * - Current and previous mouse states
 * - Button presses, movement, and wheel scroll
 *
 * Input updates are handled once per frame in `input_update()`,
 * which copies current state to previous for delta detection.
 *
 * Usage:
 * Use the public interface defined in `input.h` to interact with this system.
 */

/**
 * @brief Internal structure representing the current keyboard state.
 */
typedef struct keyboard_state {
    /**
     * @brief Array tracking whether each key is currently pressed (True/False).
     *
     * Supports up to 256 keys — sufficient for all standard keyboard inputs.
     */
    b8 keys[256];
} keyboard_state;

/**
 * @brief Internal structure representing the current mouse state.
 */
typedef struct mouse_state {
    /**
     * @brief X position of the mouse relative to client area.
     */
    i16 x;

    /**
     * @brief Y position of the mouse relative to client area.
     */
    i16 y;

    /**
     * @brief Tracks the current state of mouse buttons.
     */
    u8 buttons[BUTTON_MAX_BUTTONS];  ///< Mouse button states (pressed/released)
} mouse_state;

/**
 * @brief Global input state combining keyboard and mouse data.
 */
typedef struct input_state {
    /**
     * @brief State of the keyboard at the current frame.
     */
    keyboard_state keyboard_current;

    /**
     * @brief State of the keyboard at the previous frame.
     */
    keyboard_state keyboard_previous;

    /**
     * @brief State of the mouse at the current frame.
     */
    mouse_state mouse_current;

    /**
     * @brief State of the mouse at the previous frame.
     */
    mouse_state mouse_previous;
} input_state;

// Static global instance of the input state
static b8 initialized = False;
static input_state state = {};

/**
 * @brief Initializes the input system.
 *
 * Sets internal state to zeroed memory and marks the system as initialized.
 */
void input_initialize() {
    kzero_memory(&state, sizeof(input_state));
    initialized = True;
    KINFO("Input subsystem initialized.");
}

/**
 * @brief Shuts down the input system.
 *
 * Currently just sets the initialized flag to False. Can be extended to clean up resources.
 */
void input_shutdown() {
    // TODO: Add shutdown routines when needed.
    initialized = False;
}

/**
 * @brief Updates input state from the previous to current frame.
 *
 * Copies current keyboard/mouse states into previous states to support delta checks.
 *
 * @param delta_time Time since the last update (unused here but follows pattern).
 */
void input_update(f64 delta_time) {
    if (!initialized) {
        return;
    }

    // Copy current states to previous states for delta comparison
    kcopy_memory(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    kcopy_memory(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));
}

/**
 * @brief Processes a key press or release event.
 *
 * Fires an appropriate event (`EVENT_CODE_KEY_PRESSED` or `EVENT_CODE_KEY_RELEASED`)
 * only if the key state has changed.
 *
 * @param key The key being processed.
 * @param pressed True if the key is now pressed; False if released.
 */
void input_process_key(keys key, b8 pressed) {
    // Only handle changes in state
    if (state.keyboard_current.keys[key] != pressed) {
        // Update internal state
        state.keyboard_current.keys[key] = pressed;

        // Fire event so listeners can respond
        event_context context;
        context.data.u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

/**
 * @brief Processes a mouse button press or release.
 *
 * Fires an appropriate event (`EVENT_CODE_BUTTON_PRESSED` or `EVENT_CODE_BUTTON_RELEASED`)
 * only if the button state has changed.
 *
 * @param button The mouse button being processed.
 * @param pressed True if the button is now pressed; False if released.
 */
void input_process_button(buttons button, b8 pressed) {
    // Only process if state changed
    if (state.mouse_current.buttons[button] != pressed) {
        // Update internal state
        state.mouse_current.buttons[button] = pressed;

        // Notify listeners
        event_context context;
        context.data.u16[0] = button;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

/**
 * @brief Processes mouse movement and updates internal state.
 *
 * Fires `EVENT_CODE_MOUSE_MOVED` if the position has changed.
 *
 * @param x New X coordinate of the mouse.
 * @param y New Y coordinate of the mouse.
 */
void input_process_mouse_move(i16 x, i16 y) {
    // Only update if position actually changed
    if (state.mouse_current.x != x || state.mouse_current.y != y) {
        // DEBUG INFO
        KDEBUG("Mouse pos: %i, %i!", x, y);

        // Update internal state
        state.mouse_current.x = x;
        state.mouse_current.y = y;

        // Fire mouse move event
        event_context context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, context);
    }
}

/**
 * @brief Processes mouse wheel scroll input.
 *
 * Fires `EVENT_CODE_MOUSE_WHEEL` with the scroll amount.
 *
 * @param z_delta Delta value indicating scroll direction and magnitude.
 */
void input_process_mouse_wheel(i8 z_delta) {
    // No internal state to update — fire event directly
    event_context context;
    context.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

/**
 * @brief Checks if a key is currently pressed.
 *
 * @param key The key to check.
 * @return True if the key is currently down; False otherwise.
 */
b8 input_is_key_down(keys key) {
    if (!initialized) {
        return False;
    }
    return state.keyboard_current.keys[key] == True;
}

/**
 * @brief Checks if a key is currently released.
 *
 * @param key The key to check.
 * @return True if the key is currently up; False otherwise.
 */
b8 input_is_key_up(keys key) {
    if (!initialized) {
        return True;
    }
    return state.keyboard_current.keys[key] == False;
}

/**
 * @brief Checks if a key was pressed during the previous frame.
 *
 * Useful for detecting single-frame input actions.
 *
 * @param key The key to check.
 * @return True if the key was pressed last frame; False otherwise.
 */
b8 input_was_key_down(keys key) {
    if (!initialized) {
        return False;
    }
    return state.keyboard_previous.keys[key] == True;
}

/**
 * @brief Checks if a key was released during the previous frame.
 *
 * Useful for detecting single-frame input releases.
 *
 * @param key The key to check.
 * @return True if the key was released last frame; False otherwise.
 */
b8 input_was_key_up(keys key) {
    if (!initialized) {
        return True;
    }
    return state.keyboard_previous.keys[key] == False;
}

/**
 * @brief Checks if a mouse button is currently pressed.
 *
 * @param button The mouse button to check.
 * @return True if the button is currently down; False otherwise.
 */
b8 input_is_button_down(buttons button) {
    if (!initialized) {
        return False;
    }
    return state.mouse_current.buttons[button] == True;
}

/**
 * @brief Checks if a mouse button is currently released.
 *
 * @param button The mouse button to check.
 * @return True if the button is currently up; False otherwise.
 */
b8 input_is_button_up(buttons button) {
    if (!initialized) {
        return True;
    }
    return state.mouse_current.buttons[button] == False;
}

/**
 * @brief Checks if a mouse button was pressed during the previous frame.
 *
 * Useful for detecting single-frame clicks.
 *
 * @param button The mouse button to check.
 * @return True if the button was pressed last frame; False otherwise.
 */
b8 input_was_button_down(buttons button) {
    if (!initialized) {
        return False;
    }
    return state.mouse_previous.buttons[button] == True;
}

/**
 * @brief Checks if a mouse button was released during the previous frame.
 *
 * Useful for detecting single-frame button releases.
 *
 * @param button The mouse button to check.
 * @return True if the button was released last frame; False otherwise.
 */
b8 input_was_button_up(buttons button) {
    if (!initialized) {
        return True;
    }
    return state.mouse_previous.buttons[button] == False;
}

/**
 * @brief Gets the current mouse position.
 *
 * @param x Pointer to store the current X position.
 * @param y Pointer to store the current Y position.
 */
void input_get_mouse_position(i32* x, i32* y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_current.x;
    *y = state.mouse_current.y;
}

/**
 * @brief Gets the mouse position from the previous frame.
 *
 * Useful for computing deltas like cursor speed or drag distance.
 *
 * @param x Pointer to store the previous X position.
 * @param y Pointer to store the previous Y position.
 */
void input_get_previous_mouse_position(i32* x, i32* y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_previous.x;
    *y = state.mouse_previous.y;
}