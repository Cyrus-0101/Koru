#define _POSIX_C_SOURCE 199309L  // Enables clock_gettime, CLOCK_MONOTONIC

#include "platform.h"
#include "containers/darray.h"
#include "core/input.h"
#include "core/logger.h"  // Custom logging system used in engine/application
#include "core/event.h"

/**
 * @file platform_linux.c
 * @brief Linux-specific implementation of the platform abstraction layer.
 *
 * Implements all platform-dependent functionality for Linux-based systems including:
 * - Window creation and event handling
 * - Input device polling
 * - Memory allocation and manipulation
 * - Timing and sleep functions
 */

// Linux platform layer.

// This entire block will only compile if targeting Linux (KPLATFORM_LINUX is defined)
#if KPLATFORM_LINUX

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

#define VK_USE_PLATFORM_XCB_KHR  // For surface creation
#include <vulkan/vulkan.h>
#include "renderer/vulkan/vulkan_types.inl"

/**
 * @struct internal_state
 * @brief Platform-specific state for Linux/XCB-based windowing system.
 *
 * Stores low-level handles and atoms needed to manage:
 * - Window creation and destruction
 * - Event handling (keyboard, mouse, window close)
 * - Interaction with the X Window Manager
 */
typedef struct platform_state {
    /**
     * @brief Pointer to the X11 display connection.
     *
     * Used to interact with the X server using legacy Xlib functions.
     */
    Display* display;

    /**
     * @brief Connection to the X server using XCB (X protocol C Binding).
     *
     * Provides modern, asynchronous access to the X Window System.
     */
    xcb_connection_t* connection;

    /**
     * @brief Handle to the created application window.
     *
     * Used to send commands like map/unmap, resize, and destroy.
     */
    xcb_window_t window;

    /**
     * @brief Pointer to the current screen information.
     *
     * Contains resolution, color depth, and root window info.
     */
    xcb_screen_t* screen;

    /**
     * @brief Atom representing the WM_PROTOCOLS message type.
     *
     * Used to register supported window manager protocols.
     */
    xcb_atom_t wm_protocols;

    /**
     * @brief Atom representing the WM_DELETE_WINDOW protocol.
     *
     * Used to detect when the user clicks the close button.
     */
    xcb_atom_t wm_delete_win;

    /**
     * @brief Vulkan surface handle.
     *
     * Used for rendering with Vulkan in this window.
     */
    VkSurfaceKHR surface;
} platform_state;

static platform_state* state_ptr;  ///< Global pointer to the platform state

/**
 * @brief Translates an X11 keycode into a Koru engine key code.
 *
 * Converts raw X11/XCB key symbols into a unified key enum used by the engine.
 *
 * @param x_keycode The X11/XCB key symbol to translate.
 * @return The corresponding `keys` enum value, or 0 if unknown.
 */
keys translate_keycode(u32 x_keycode);

/**
 * @brief Initializes the Linux platform layer and creates an XCB-based window.
 *
 * Sets up the X11 display, creates a window, registers event handlers,
 * and configures basic window properties like title and size.
 *
 * @param plat_state A pointer to the platform_state structure to initialize.
 * @param application_name The title of the window.
 * @param x The initial x position of the window.
 * @param y The initial y position of the window.
 * @param width The width of the client area.
 * @param height The height of the client area.
 * @return True if successful; False otherwise.
 */
b8 platform_system_startup(
    u64* memory_requirement,
    void* state,
    const char* application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height) {
    *memory_requirement = sizeof(platform_state);
    if (state == 0) {
        return True;
    }

    state_ptr = state;
    // Connect to X
    state_ptr->display = XOpenDisplay(NULL);

    // Disable keyboard auto-repeat globally (games usually want discrete key presses)
    XAutoRepeatOff(state_ptr->display);

    // Get the modern XCB connection from the old Xlib Display
    state_ptr->connection = XGetXCBConnection(state_ptr->display);

    // Check if there was any error connecting
    if (xcb_connection_has_error(state_ptr->connection)) {
        KFATAL("Failed to connect to X server via XCB.");
        return False;
    }

    // Retrieve the setup data from the connection, akin to getting configuration
    const struct xcb_setup_t* setup = xcb_get_setup(state_ptr->connection);

    // Iterate through available screens (monitors) using an iterator
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);

    // We're choosing the first screen
    int screen_p = 0;

    for (i32 s = screen_p; s > 0; s--) {
        xcb_screen_next(&it);  // Move iterator forward
    }

    // Assign the selected screen
    state_ptr->screen = it.data;

    // Generate a unique XID for the window created
    state_ptr->window = xcb_generate_id(state_ptr->connection);

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
    u32 value_list[] = {state_ptr->screen->black_pixel, event_values};

    /**
     * Create the window using XCB
     */
    xcb_void_cookie_t cookie = xcb_create_window(
        state_ptr->connection,
        XCB_COPY_FROM_PARENT,            // Use same depth as parent
        state_ptr->window,               // Parent window (root window of screen)
        state_ptr->screen->root,         // parent
        x,                               // x position
        y,                               // y position
        width,                           // width
        height,                          // height
        0,                               // No border
        XCB_WINDOW_CLASS_INPUT_OUTPUT,   // Can both draw and receive input
        state_ptr->screen->root_visual,  // Same color format as root window
        event_mask,                      // Mask for attributes
        value_list);                     // Values for those attributes

    // Set the window title
    xcb_change_property(
        state_ptr->connection,
        XCB_PROP_MODE_REPLACE,  // Replace existing property
        state_ptr->window,
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
        state_ptr->connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
        state_ptr->connection,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS");

    // Wait for replies
    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(
        state_ptr->connection,
        wm_delete_cookie,
        NULL);
    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(
        state_ptr->connection,
        wm_protocols_cookie,
        NULL);

    // Save atoms to internal state
    state_ptr->wm_delete_win = wm_delete_reply->atom;
    state_ptr->wm_protocols = wm_protocols_reply->atom;

    // Let the window manager know we support WM_DELETE_WINDOW protocol
    xcb_change_property(
        state_ptr->connection,
        XCB_PROP_MODE_REPLACE,
        state_ptr->window,
        wm_protocols_reply->atom,
        4,                        // Format
        32,                       // 32-bit values
        1,                        // One element
        &wm_delete_reply->atom);  // Pointer to atom

    // Map the window to the screen, make the window visible
    xcb_map_window(state_ptr->connection, state_ptr->window);

    // Send all previous commands to the X server
    // Flush the stream
    i32 stream_result = xcb_flush(state_ptr->connection);
    if (stream_result <= 0) {
        KFATAL("An error occurred when flusing the stream: %d", stream_result);
        return False;
    }

    return True;
}

/**
 * @brief Shuts down the platform layer and cleans up all allocated resources.
 *
 * Destroys the window, disconnects from the X server, and restores system settings
 * such as keyboard auto-repeat.
 *
 * @param plat_state A pointer to the platform_state structure to shut down.
 */
void platform_system_shutdown(void* plat_state) {
    if (state_ptr) {
        // Turn key repeat back on since it was turned off globally, this is global for the OS, almost unbelievable.
        XAutoRepeatOn(state_ptr->display);

        // Destroy the window
        xcb_destroy_window(state_ptr->connection, state_ptr->window);
    }
}

/**
 * @brief Processes all pending platform events (input, window close, etc.).
 *
 * Polls the XCB event queue and dispatches relevant events to the engine's
 * input and event systems.
 *
 * @param plat_state A pointer to the platform_state structure.
 * @return True if the application should continue running; False if quit is requested.
 */
b8 platform_pump_messages() {
    if (state_ptr) {
        xcb_generic_event_t* event;
        xcb_client_message_event_t* cm;

        b8 quit_flagged = False;

        // Poll for all pending events
        while (event != 0) {
            event = xcb_poll_for_event(state_ptr->connection);

            // Handle only one event per loop iteration
            if (event == 0) {
                break;
            }

            // Handle Input events
            switch (event->response_type & ~0x80) {
                case XCB_KEY_PRESS:
                case XCB_KEY_RELEASE: {
                    // Key press event - xcb_key_press_event_t / xcb_key_release_event_t are similar
                    xcb_key_press_event_t* kb_event = (xcb_key_press_event_t*)event;
                    b8 pressed = event->response_type == XCB_KEY_PRESS;

                    xcb_keycode_t code = kb_event->detail;

                    KeySym key_sym = XkbKeycodeToKeysym(
                        state_ptr->display,
                        (KeyCode)code,  // event.xkey.keycode
                        0,
                        code & ShiftMask ? 1 : 0);

                    keys key = translate_keycode(key_sym);

                    // Pass to the input subsystem for processing.
                    input_process_key(key, pressed);
                } break;

                case XCB_BUTTON_PRESS:
                case XCB_BUTTON_RELEASE: {
                    xcb_button_press_event_t* mouse_event = (xcb_button_press_event_t*)event;

                    b8 pressed = event->response_type == XCB_BUTTON_PRESS;

                    buttons mouse_button = BUTTON_MAX_BUTTONS;

                    switch (mouse_event->detail) {
                        case XCB_BUTTON_INDEX_1:
                            mouse_button = BUTTON_LEFT;

                            break;
                        case XCB_BUTTON_INDEX_2:
                            mouse_button = BUTTON_MIDDLE;

                            break;
                        case XCB_BUTTON_INDEX_3:
                            mouse_button = BUTTON_RIGHT;

                            break;
                    }

                    // Pass over to the input subsystem.
                    if (mouse_button != BUTTON_MAX_BUTTONS) {
                        input_process_button(mouse_button, pressed);
                    }
                } break;

                case XCB_MOTION_NOTIFY: {
                    // Mouse move
                    xcb_motion_notify_event_t* move_event = (xcb_motion_notify_event_t*)event;

                    // Pass to the input subsystem.
                    input_process_mouse_move(move_event->event_x, move_event->event_y);
                } break;

                case XCB_CONFIGURE_NOTIFY: {
                    /*
                     * Resizing - note that this is also triggered by moving the window, but should be
                     * passed anyway since a change in the x/y could mean an upper-left resize.
                     * The application layer can decide what to do with this.
                     */
                    xcb_configure_notify_event_t* configure_event = (xcb_configure_notify_event_t*)event;

                    /*
                     * Fire the event. The application layer should pick this up,
                     * but not handle it as it shouldn't be visible to other parts
                     * of the application.
                     */
                    event_context context;

                    context.data.u16[0] = configure_event->width;
                    context.data.u16[1] = configure_event->height;

                    event_fire(EVENT_CODE_RESIZED, 0, context);
                } break;

                case XCB_CLIENT_MESSAGE: {
                    cm = (xcb_client_message_event_t*)event;

                    // Window close
                    // If the message is WM_DELETE_WINDOW, user wants to quit
                    if (cm->data.data32[0] == state_ptr->wm_delete_win) {
                        quit_flagged = True;
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
    return True;  // If no state_ptr, assume no events to process
}

/**
 * @brief Allocates memory using standard malloc.
 *
 * This is a simple wrapper around malloc for use in early initialization before
 * the custom memory system is available.
 *
 * @param size The number of bytes to allocate.
 * @param aligned Ignored here; included for interface compatibility.
 * @return A pointer to the newly allocated block of memory.
 */
void* platform_allocate(u64 size, b8 aligned) {
    return malloc(size);
}

/**
 * @brief Frees a previously allocated block of memory.
 *
 * Wrapper around free() to match allocation interface.
 *
 * @param block A pointer to the memory block to free.
 * @param aligned Ignored here; included for interface compatibility.
 */
void platform_free(void* block, b8 aligned) {
    free(block);
}

/**
 * @brief Fills a block of memory with zeros.
 *
 * Equivalent to memset(..., 0, ...).
 *
 * @param block A pointer to the memory block.
 * @param size The number of bytes to zero.
 * @return A pointer to the zeroed memory block.
 */
void* platform_zero_memory(void* block, u64 size) {
    return memset(block, 0, size);
}

/**
 * @brief Copies data from one memory block to another.
 *
 * Equivalent to memcpy().
 *
 * @param dest A pointer to the destination memory block.
 * @param source A pointer to the source memory block.
 * @param size The number of bytes to copy.
 * @return A pointer to the destination memory block.
 */
void* platform_copy_memory(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size);
}

/**
 * @brief Fills a block of memory with a specific byte value.
 *
 * Equivalent to memset().
 *
 * @param dest A pointer to the memory block.
 * @param value The byte value to set.
 * @param size The number of bytes to fill.
 * @return A pointer to the modified memory block.
 */
void* platform_set_memory(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

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

f64 platform_get_absolute_time() {
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);

    return now.tv_sec + now.tv_nsec * 0.000000001;
}

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

// Surface creation for VulkanAdd commentMore actions
b8 platform_create_vulkan_surface(vulkan_context* context) {
    if(!state_ptr) {
        return False;
    }

    VkXcbSurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    create_info.connection = state_ptr->connection;
    create_info.window = state_ptr->window;

    VkResult result = vkCreateXcbSurfaceKHR(
        context->instance,
        &create_info,
        context->allocator,
        &state_ptr->surface);
    if (result != VK_SUCCESS) {
        KFATAL("Vulkan surface creation failed.");
        return False;
    }

    context->surface = state_ptr->surface;
    return True;
}

/**
 * @brief Appends platform-specific Vulkan extension names to the provided array.
 *
 * Adds the required surface extensions needed for creating a Vulkan instance
 * that can render to an XCB-based window.
 *
 * @param names_darray A pointer to a dynamic array of `const char*` where extension names will be appended.
 */
void platform_get_required_extension_names(const char*** names_darray) {
    darray_push(*names_darray, &"VK_KHR_xcb_surface");  // VK_KHR_xlib_surface?
}

keys translate_keycode(u32 x_keycode) {
    switch (x_keycode) {
        case XK_BackSpace:
            return KEY_BACKSPACE;
        case XK_Return:
            return KEY_ENTER;
        case XK_Tab:
            return KEY_TAB;
            // case XK_Shift: return KEY_SHIFT;
            // case XK_Control: return KEY_CONTROL;

        case XK_Pause:
            return KEY_PAUSE;
        case XK_Caps_Lock:
            return KEY_CAPITAL;

        case XK_Escape:
            return KEY_ESCAPE;

            // Not supported
            // case : return KEY_CONVERT;
            // case : return KEY_NONCONVERT;
            // case : return KEY_ACCEPT;

        case XK_Mode_switch:
            return KEY_MODECHANGE;

        case XK_space:
            return KEY_SPACE;
        case XK_Prior:
            return KEY_PRIOR;
        case XK_Next:
            return KEY_NEXT;
        case XK_End:
            return KEY_END;
        case XK_Home:
            return KEY_HOME;
        case XK_Left:
            return KEY_LEFT;
        case XK_Up:
            return KEY_UP;
        case XK_Right:
            return KEY_RIGHT;
        case XK_Down:
            return KEY_DOWN;
        case XK_Select:
            return KEY_SELECT;
        case XK_Print:
            return KEY_PRINT;
        case XK_Execute:
            return KEY_EXECUTE;
        // case XK_snapshot: return KEY_SNAPSHOT; // not supported
        case XK_Insert:
            return KEY_INSERT;
        case XK_Delete:
            return KEY_DELETE;
        case XK_Help:
            return KEY_HELP;

        case XK_Meta_L:
            return KEY_LWIN;  // TODO: not sure this is right
        case XK_Meta_R:
            return KEY_RWIN;
            // case XK_apps: return KEY_APPS; // not supported

            // case XK_sleep: return KEY_SLEEP; //not supported

        case XK_KP_0:
        case XK_0:
            return KEY_NUMPAD0;
        case XK_KP_1:
        case XK_1:
            return KEY_NUMPAD1;
        case XK_KP_2:
        case XK_2:
            return KEY_NUMPAD2;
        case XK_KP_3:
        case XK_3:
            return KEY_NUMPAD3;
        case XK_KP_4:
        case XK_4:
            return KEY_NUMPAD4;
        case XK_KP_5:
        case XK_5:
            return KEY_NUMPAD5;
        case XK_KP_6:
        case XK_6:
            return KEY_NUMPAD6;
        case XK_KP_7:
        case XK_7:
            return KEY_NUMPAD7;
        case XK_KP_8:
        case XK_8:
            return KEY_NUMPAD8;
        case XK_KP_9:
        case XK_9:
            return KEY_NUMPAD9;
        case XK_multiply:
            return KEY_MULTIPLY;
        case XK_KP_Add:
            return KEY_ADD;
        case XK_KP_Separator:
            return KEY_SEPARATOR;
        case XK_KP_Subtract:
            return KEY_SUBTRACT;
        case XK_KP_Decimal:
            return KEY_DECIMAL;
        case XK_KP_Divide:
            return KEY_DIVIDE;
        case XK_F1:
            return KEY_F1;
        case XK_F2:
            return KEY_F2;
        case XK_F3:
            return KEY_F3;
        case XK_F4:
            return KEY_F4;
        case XK_F5:
            return KEY_F5;
        case XK_F6:
            return KEY_F6;
        case XK_F7:
            return KEY_F7;
        case XK_F8:
            return KEY_F8;
        case XK_F9:
            return KEY_F9;
        case XK_F10:
            return KEY_F10;
        case XK_F11:
            return KEY_F11;
        case XK_F12:
            return KEY_F12;
        case XK_F13:
            return KEY_F13;
        case XK_F14:
            return KEY_F14;
        case XK_F15:
            return KEY_F15;
        case XK_F16:
            return KEY_F16;
        case XK_F17:
            return KEY_F17;
        case XK_F18:
            return KEY_F18;
        case XK_F19:
            return KEY_F19;
        case XK_F20:
            return KEY_F20;
        case XK_F21:
            return KEY_F21;
        case XK_F22:
            return KEY_F22;
        case XK_F23:
            return KEY_F23;
        case XK_F24:
            return KEY_F24;

        case XK_Num_Lock:
            return KEY_NUMLOCK;
        case XK_Scroll_Lock:
            return KEY_SCROLL;

        case XK_KP_Equal:
            return KEY_NUMPAD_EQUAL;

        case XK_Shift_L:
            return KEY_LSHIFT;
        case XK_Shift_R:
            return KEY_RSHIFT;
        case XK_Control_L:
            return KEY_LCONTROL;
        case XK_Control_R:
            return KEY_RCONTROL;
        case XK_Alt_L:
            return KEY_LALT;
        case XK_Alt_R:
            return KEY_RALT;

        case XK_semicolon:
            return KEY_SEMICOLON;
        case XK_plus:
            return KEY_PLUS;
        case XK_comma:
            return KEY_COMMA;
        case XK_minus:
            return KEY_MINUS;
        case XK_period:
            return KEY_PERIOD;
        case XK_slash:
            return KEY_SLASH;
        case XK_grave:
            return KEY_GRAVE;

        case XK_a:
        case XK_A:
            return KEY_A;
        case XK_b:
        case XK_B:
            return KEY_B;
        case XK_c:
        case XK_C:
            return KEY_C;
        case XK_d:
        case XK_D:
            return KEY_D;
        case XK_e:
        case XK_E:
            return KEY_E;
        case XK_f:
        case XK_F:
            return KEY_F;
        case XK_g:
        case XK_G:
            return KEY_G;
        case XK_h:
        case XK_H:
            return KEY_H;
        case XK_i:
        case XK_I:
            return KEY_I;
        case XK_j:
        case XK_J:
            return KEY_J;
        case XK_k:
        case XK_K:
            return KEY_K;
        case XK_l:
        case XK_L:
            return KEY_L;
        case XK_m:
        case XK_M:
            return KEY_M;
        case XK_n:
        case XK_N:
            return KEY_N;
        case XK_o:
        case XK_O:
            return KEY_O;
        case XK_p:
        case XK_P:
            return KEY_P;
        case XK_q:
        case XK_Q:
            return KEY_Q;
        case XK_r:
        case XK_R:
            return KEY_R;
        case XK_s:
        case XK_S:
            return KEY_S;
        case XK_t:
        case XK_T:
            return KEY_T;
        case XK_u:
        case XK_U:
            return KEY_U;
        case XK_v:
        case XK_V:
            return KEY_V;
        case XK_w:
        case XK_W:
            return KEY_W;
        case XK_x:
        case XK_X:
            return KEY_X;
        case XK_y:
        case XK_Y:
            return KEY_Y;
        case XK_z:
        case XK_Z:
            return KEY_Z;

        default:
            return 0;
    }
}

#endif