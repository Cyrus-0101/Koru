#define _POSIX_C_SOURCE 199309L  // Enables clock_gettime, CLOCK_MONOTONIC

#include "platform.h"

// Linux platform layer.
// This entire block will only compile if targeting Linux (KPLATFORM_LINUX is defined)
#if KPLATFORM_LINUX

#include "core/logger.h"  // Custom logging system used in engine/application

// X Window System headers using XCB (X protocol C Binding)
#include <xcb/xcb.h>
#include <X11/keysym.h>    // For key symbol definitions
#include <X11/XKBlib.h>    // X Keyboard Extension library, needed for XAutoRepeatOff/On
#include <X11/Xlib.h>      // Legacy X11 library; required for XOpenDisplay
#include <X11/Xlib-xcb.h>  // Interop between Xlib and XCB
#include <sys/time.h>      // Time-related functions (e.g., gettimeofday)

// Conditional includes based on POSIX standard version
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  // nanosleep for precise sleep
#else
#include <unistd.h>  // usleep (less precise but older alternative)
#endif

// Standard C libraries
#include <stdlib.h>  // malloc, free, etc.
#include <stdio.h>   // printf
#include <string.h>  // memset, memcpy, etc.

/**
 * A struct groups related data together.
 * Holds all low-level info about our window and connection.
 */
typedef struct internal_state {
    Display* display;              // Old-style X11 display
    xcb_connection_t* connection;  // XCB connection to X server
    xcb_window_t window;           // Handle to our window
    xcb_screen_t* screen;          // Screen info
    xcb_atom_t wm_protocols;       // Atom for WM_PROTOCOLS
    xcb_atom_t wm_delete_win;      // Atom for WM_DELETE_WINDOW
} internal_state;

/**
 * Starts up the platform-specific parts (window creation, input setup, etc.)
 *
 * @param plat_state - Platform state structure that we initialize
 * @param application_name - Title of the window
 * @param x/y - Position of the window
 * @param width/height - Dimensions of the window
 * @return TRUE if successful
 */
b8 platform_startup(
    platform_state* plat_state,
    const char* application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height) {
    // Create the internal state. Allocate memory for internal_state using malloc
    plat_state->internal_state = malloc(sizeof(internal_state));

    // Cast the void pointer to a typed pointer
    internal_state* state = (internal_state*)plat_state->internal_state;

    // Open a connection to the X11 server
    state->display = XOpenDisplay(NULL);  // NULL means default display

    // Disable keyboard auto-repeat globally (games usually want discrete key presses)
    XAutoRepeatOff(state->display);

    // Get the modern XCB connection from the old Xlib Display
    state->connection = XGetXCBConnection(state->display);

    // Check if there was any error connecting
    if (xcb_connection_has_error(state->connection)) {
        KFATAL("Failed to connect to X server via XCB.");
        return FALSE;
    }

    // Retrieve the setup data from the connection, akin to getting configuration
    const struct xcb_setup_t* setup = xcb_get_setup(state->connection);

    // Iterate through available screens (monitors) using an iterator
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);

    // We're choosing the first screen
    int screen_p = 0;

    for (i32 s = screen_p; s > 0; s--) {
        xcb_screen_next(&it);  // Move iterator forward
    }

    // Assign the selected screen
    state->screen = it.data;

    // Generate a unique XID for the window created
    state->window = xcb_generate_id(state->connection);

    // Register event types.
    // XCB_CW_BACK_PIXEL = filling then window bg with a single colour
    // XCB_CW_EVENT_MASK is required.
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // Listen for keyboard and mouse buttons events
    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    // Values to be sent over XCB (bg colour, events)
    u32 value_list[] = {state->screen->black_pixel, event_values};

    /**
     * Create the window using XCB
     */
    xcb_void_cookie_t cookie = xcb_create_window(
        state->connection,
        XCB_COPY_FROM_PARENT,           // Use same depth as parent
        state->window,                  // Parent window (root window of screen)
        state->screen->root,            // parent
        x,                              // x position
        y,                              // y position
        width,                          // width
        height,                         // height
        0,                              // No border
        XCB_WINDOW_CLASS_INPUT_OUTPUT,  // Can both draw and receive input
        state->screen->root_visual,     // Same color format as root window
        event_mask,                     // Mask for attributes
        value_list);                    // Values for those attributes

    // Set the window title
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,  // Replace existing property
        state->window,
        XCB_ATOM_WM_NAME,          // Property name (window title)
        XCB_ATOM_STRING,           // Type of data
        8,                         // Format: data should be viewed 8 bits at a time
        strlen(application_name),  // Length of string
        application_name);         // The actual title string

    /**
     * Ask the window manager what atom corresponds to WM_DELETE_WINDOW
     * This is how the OS tells us the user wants to close the window
     */
    xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS");

    // Wait for replies
    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(
        state->connection,
        wm_delete_cookie,
        NULL);
    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(
        state->connection,
        wm_protocols_cookie,
        NULL);

    // Save atoms to internal state
    state->wm_delete_win = wm_delete_reply->atom;
    state->wm_protocols = wm_protocols_reply->atom;

    // Let the window manager know we support WM_DELETE_WINDOW protocol
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        wm_protocols_reply->atom,
        4,                        // Format
        32,                       // 32-bit values
        1,                        // One element
        &wm_delete_reply->atom);  // Pointer to atom

    // Map the window to the screen, make the window visible
    xcb_map_window(state->connection, state->window);

    // Send all previous commands to the X server
    // Flush the stream
    i32 stream_result = xcb_flush(state->connection);
    if (stream_result <= 0) {
        KFATAL("An error occurred when flusing the stream: %d", stream_result);
        return FALSE;
    }

    return TRUE;
}

/**
 * Shuts down the platform layer, cleans up resources
 */
void platform_shutdown(platform_state* plat_state) {
    // Simply cold-cast to the known type.
    internal_state* state = (internal_state*)plat_state->internal_state;

    // // Turn key repeat back on since it was turned off globally, this is global for the OS... just... wow.
    XAutoRepeatOn(state->display);

    // Destroy the window
    xcb_destroy_window(state->connection, state->window);
}

/**
 * Processes incoming events (keyboard, mouse, window close, etc.)
 * Returns TRUE if app should continue running
 */
b8 platform_pump_messages(platform_state* plat_state) {
    // Simply cold-cast to the known type.
    internal_state* state = (internal_state*)plat_state->internal_state;

    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;

    b8 quit_flagged = FALSE;

    // Poll for all pending events
    while ((event = xcb_poll_for_event(state->connection))) {
        // Handle only one event per loop iteration
        if (!event) {
            break;
        }

        // Handle Input events
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                // TODO: Key presses and releases
            } break;

            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                // TODO: Mouse button presses and releases handling
            } break;

            case XCB_MOTION_NOTIFY:
                // TODO: Mouse movement
                break;

            case XCB_CONFIGURE_NOTIFY: {
                // TODO: Resizing
            } break;

            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t*)event;

                // Window close
                // If the message is WM_DELETE_WINDOW, user wants to quit
                if (cm->data.data32[0] == state->wm_delete_win) {
                    quit_flagged = TRUE;
                }
            } break;

            default:
                // Ignore unknown events
                break;
        }

        free(event);  // Free the event after processing
    }

    return !quit_flagged;
}

/**
 * Memory Allocation wrappers
 * These mimic JavaScript's dynamic memory behavior
 */
void* platform_allocate(u64 size, b8 aligned) {
    return malloc(size);
}

void platform_free(void* block, b8 aligned) {
    free(block);
}

void* platform_zero_memory(void* block, u64 size) {
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size);
}

void* platform_set_memory(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

/**
 * Console output with colors (like console.log with styles)
 */
void platform_console_write(const char* message, u8 colour) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[colour], message);
}

void platform_console_write_error(const char* message, u8 colour) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[colour], message);
}

/**
 * Gets current time in seconds
 */
f64 platform_get_absolute_time() {
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);

    return now.tv_sec + now.tv_nsec * 0.000000001;
}

/**
 * Sleeps for a number of milliseconds
 * Uses either nanosleep or usleep depending on system capabilities
 */
void platform_sleep(u64 ms) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000 * 1000;

    nanosleep(&ts, 0);
#else
    if (ms >= 1000) {
        sleep(ms / 1000);
    }

    usleep((ms % 1000) * 1000);
#endif
}

#endif