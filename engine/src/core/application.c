#include "application.h"
#include "game_types.h"
#include "core/event.h"
#include "core/logger.h"
#include "core/kmemory.h"
#include "platform/platform.h"
#include "core/input.h"

/**
 * @file application.c
 * @brief Implementation of the core application lifecycle functions.
 *
 * This module implements the functions defined in `application.h`.
 * It initializes the platform layer, starts the game loop,
 * and manages clean shutdown procedures.
 *
 * Responsibilities:
 * - Initialize subsystems like memory and logging
 * - Start and run the game loop
 * - Gracefully shut down the engine
 */

/**
 * @brief Internal state structure for the application.
 *
 * Tracks:
 * - Application lifecycle (running/suspended)
 * - Platform-specific data
 * - Window dimensions
 * - Timing information for frame updates
 */
typedef struct application_state {
    game* game_inst;
    /**
     * @brief Indicates whether the application is currently running.
     */
    b8 is_running;

    /**
     * @brief Indicates whether the application is currently suspended.
     */
    b8 is_suspended;

    /**
     * @brief Opaque platform-specific state used for windowing, input, etc.
     */
    platform_state platform;

    /**
     * @brief The width of the application window client area.
     */
    i16 width;

    /**
     * @brief The height of the application window client area.
     */
    i16 height;

    /**
     * @brief Timestamp of the last processed frame for delta time calculation.
     */
    f64 last_time;
} application_state;

/**
 * @brief Global flag indicating if the application has already been initialized.
 */
static b8 initialized = FALSE;

/**
 * @brief Global instance of the application state.
 */
static application_state app_state;

/**
 * @brief Global event handler for application-level events.
 *
 * This function listens for high-level system events like:
 * - Application quit request (`EVENT_CODE_APPLICATION_QUIT`)
 *
 * When such an event is received, it updates internal state to gracefully exit.
 *
 * @param code The event code that was fired.
 * @param sender A pointer to the object that sent the event (can be NULL).
 * @param listener_inst A pointer to the listener instance (unused here).
 * @param context Event-specific data (unused here).
 * @return TRUE if the event was handled; FALSE otherwise.
 */
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);

/**
 * @brief Handles keyboard input at the application level.
 *
 * Responds to `EVENT_CODE_KEY_PRESSED` and `EVENT_CODE_KEY_RELEASED` events.
 *
 * Special keys like Escape can trigger application-wide actions,
 * while others are logged for debugging purposes.
 *
 * @param code The event code (e.g., key pressed/released).
 * @param sender A pointer to the object that triggered the event (unused here).
 * @param listener_inst A pointer to the listener instance (unused here).
 * @param context Event-specific data containing key information.
 * @return TRUE if the event was handled; FALSE otherwise.
 */
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

/**
 * @brief Initializes the application with the provided configuration.
 *
 * This function must be called once before calling application_run().
 * It sets up internal systems such as logging, initializes the platform layer,
 * and prepares the main application loop.
 *
 * @param config A pointer to an application_config struct containing initialization settings.
 * @return TRUE if initialization was successful; FALSE otherwise.
 */
b8 application_create(game* game_inst) {
    if (initialized) {
        KERROR("application_create() called more than once");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Initialize Subsystem
    initialize_logging();
    input_initialize();

    // TO-DO: Remove this
    KFATAL("A test message: %f", 3.14f);
    KERROR("A test message: %f", 3.14f);
    KWARN("A test message: %f", 3.14f);
    KINFO("A test message: %f", 3.14f);
    KDEBUG("A test message: %f", 3.14f);
    KTRACE("A test message: %f", 3.14f);

    // Set initial application state
    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if (!event_initialize()) {
        KERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }

    // Register for Key press events
    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    // Start platform layer
    if (!platform_startup(&app_state.platform, game_inst->app_config.name, game_inst->app_config.start_pos_x, game_inst->app_config.start_pos_y, game_inst->app_config.start_width, game_inst->app_config.start_height)) {
        return TRUE;
    }

    // Initialize Game
    if (!app_state.game_inst->initialize(app_state.game_inst)) {
        KFATAL("Game failed to initialize");

        return FALSE;
    }

    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);

    initialized = TRUE;

    return TRUE;
}

b8 application_run() {
    // Log memory info - Memory Leak
    KINFO(get_memory_usage_str());

    while (app_state.is_running) {
        if (!platform_pump_messages(&app_state.platform)) {
            // If platform request to quit, stop running
            app_state.is_running = FALSE;
            break;
        }

        if (!app_state.is_suspended) {
            if (!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
                KFATAL("Game update failed. Shutting down!");
                app_state.is_running = FALSE;

                break;
            }

            // Call game render routine
            if (!app_state.game_inst->render(app_state.game_inst, (f32)0)) {
                KFATAL("Game render failed. Shutting down!");
                app_state.is_running = FALSE;

                break;
            }

            // NOTE: Input update/state copying should always be handled
            // after any input should be recorded; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
            input_update(0);
        }
    }

    // Ensure running state is cleared
    app_state.is_running = FALSE;

    // Shutdown event system.   
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    event_shutdown();
    input_shutdown();

    // Clean up platform resources
    platform_shutdown(&app_state.platform);

    return TRUE;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            KINFO("EVENT_CODE_APPLICATION_QUIT received, shutting down.\n");
            app_state.is_running = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            // Block anything else from processing this.
            return TRUE;
        } else if (key_code == KEY_A) {
            // Example on checking for a key
            KDEBUG("Explicit - A key pressed!");
        } else {
            KDEBUG("'%c' key pressed in window.", key_code);
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_B) {
            // Example on checking for a key
            KDEBUG("Explicit - B key released!");
        } else {
            KDEBUG("'%c' key released in window.", key_code);
        }
    }
    return FALSE;
}