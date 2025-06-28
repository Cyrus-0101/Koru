#include "application.h"
#include "game_types.h"
#include "core/event.h"
#include "core/logger.h"
#include "core/kmemory.h"
#include "platform/platform.h"
#include "core/input.h"
#include "core/clock.h"
#include "memory/linear_allocator.h"
#include "renderer/renderer_frontend.h"
#include "core/event.h"

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
     * @brief The width of the application window client area.
     */
    i16 width;

    /**
     * @brief The height of the application window client area.
     */
    i16 height;

    /**
     * @brief Clock used for timing and frame updates.
     */
    clock clock;

    /**
     * @brief Timestamp of the last processed frame for delta time calculation.
     */
    f64 last_time;

    /**
     * @brief Allocator for internal systems.
     */
    linear_allocator systems_allocator;

    u64 event_system_memory_requirement;

    void* event_system_state;

    /**
     * @brief The total memory requirement for the memory system.
     *
     * This is used to allocate memory for the memory system state.
     */
    u64 memory_system_memory_requirement;

    /**
     * @brief Pointer to the memory system state.
     *
     * This is used to manage the memory system's internal state.
     */
    void* memory_system_state;

    /**
     * @brief The total memory requirement for the logging system.
     *
     * This is used to allocate memory for the logging system state.
     */
    u64 logging_system_memory_requirement;

    /**
     * @brief Pointer to the logging system state.
     *
     * This is used to manage the logging system's internal state.
     */
    void* logging_system_state;

    u64 input_system_memory_requirement;

    void* input_system_state;

    u64 platform_system_memory_requirement;

    void* platform_system_state;

    u64 renderer_system_memory_requirement;

    void* renderer_system_state;
} application_state;

/**
 * @brief Global instance of the application state.
 */
static application_state* app_state;

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
 * @return True if the event was handled; False otherwise.
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
 * @return True if the event was handled; False otherwise.
 */
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

/**
 * @brief Handles window resize events.
 *
 * This function is registered as an event listener for 'EVENT_CODE_RESIZED' events.
 * It updates internal state, suspends/resumes rendering,
 * and forwards the resize event to the game and renderer modules.
 *
 * On Linux/X11, minimized windows report size 0x0 — this function detects that
 * and suspends rendering until the window is restored.
 *
 * @param code The event code (should be EVENT_CODE_RESIZED).
 * @param sender A pointer to the object that sent the event (unused here).
 * @param listener_inst A pointer to the listener instance (unused here).
 * @param context Event-specific data containing width and height.
 * @return True if handled; False otherwise (so other listeners can also receive the event).
 */
b8 application_on_resized(u16 code, void* sender, void* listener_inst, event_context context);

/**
 * @brief Initializes the application with the provided configuration.
 *
 * This function must be called once before calling application_run().
 * It sets up internal systems such as logging, initializes the platform layer,
 * and prepares the main application loop.
 *
 * @param config A pointer to an application_config struct containing initialization settings.
 * @return True if initialization was successful; False otherwise.
 */
b8 application_create(game* game_inst) {
    if (game_inst->application_state) {
        KERROR("application_create() called more than once");
        return False;
    }

    // TO-DO: Temporary
    game_inst->application_state = kallocate(sizeof(application_state), MEMORY_TAG_APPLICATION);
    app_state = game_inst->application_state;
    app_state->game_inst = game_inst;

    // Set initial application state
    app_state->is_running = False;
    app_state->is_suspended = False;

    // Allocate memory for the application state
    u64 systems_allocator_total_size = 64 * 1024 * 1024;  // 64 MB
    linear_allocator_create(systems_allocator_total_size, 0, &app_state->systems_allocator);

    // Initialize event system
    event_system_initialize(&app_state->event_system_memory_requirement, 0);
    app_state->event_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->event_system_memory_requirement);
    event_system_initialize(&app_state->event_system_memory_requirement, app_state->event_system_state);

    // Initialize memory system
    initialize_memory(&app_state->memory_system_memory_requirement, 0);
    app_state->memory_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->memory_system_memory_requirement);
    initialize_memory(&app_state->memory_system_memory_requirement, app_state->memory_system_state);

    // Initialize Logging Subsystem
    initialize_logging(&app_state->logging_system_memory_requirement, 0);
    app_state->logging_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->logging_system_memory_requirement);
    if (!initialize_logging(&app_state->logging_system_memory_requirement, app_state->logging_system_state)) {
        KERROR("Failed to initialize logging system. Application cannot continue. Shutting down...");
        return False;
    }

    // Initialize input system
    input_system_initialize(&app_state->input_system_memory_requirement, 0);
    app_state->input_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->input_system_memory_requirement);
    input_system_initialize(&app_state->input_system_memory_requirement, app_state->input_system_state);

    // Register for Key press events
    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_register(EVENT_CODE_RESIZED, 0, application_on_resized);

    // Start platform layer
    platform_system_startup(&app_state->platform_system_memory_requirement, 0, 0, 0, 0, 0, 0);
    app_state->platform_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->platform_system_memory_requirement);
    if (!platform_system_startup(&app_state->platform_system_memory_requirement, app_state->platform_system_state, game_inst->app_config.name, game_inst->app_config.start_pos_x, game_inst->app_config.start_pos_y, game_inst->app_config.start_width, game_inst->app_config.start_height)) {
        return False;
    }

    // Renderer startup
    renderer_system_initialize(&app_state->renderer_system_memory_requirement, 0, 0);
    app_state->renderer_system_state = linear_allocator_allocate(&app_state->systems_allocator, app_state->renderer_system_memory_requirement);
    if (!renderer_system_initialize(&app_state->renderer_system_memory_requirement, app_state->renderer_system_state, game_inst->app_config.name)) {
        KFATAL("Failed to initialize renderer. Aborting application.");

        return False;
    }

    // Initialize Game
    if (!app_state->game_inst->initialize(app_state->game_inst)) {
        KFATAL("Game failed to initialize");

        return False;
    }

    app_state->game_inst->on_resize(app_state->game_inst, app_state->width, app_state->height);

    return True;
}

b8 application_run() {
    app_state->is_running = True;

    clock_start(&app_state->clock);

    clock_update(&app_state->clock);

    app_state->last_time = app_state->clock.elapsed;

    f64 running_time = 0;

    u8 frame_count = 0;

    f64 target_frame_seconds = 1.0f / 60;

    // Log memory info - Memory Leak
    KINFO(get_memory_usage_str());

    while (app_state->is_running) {
        if (!platform_pump_messages()) {
            // If platform request to quit, stop running
            app_state->is_running = False;
        }

        if (!app_state->is_suspended) {
            // Update clock and get delta time.
            clock_update(&app_state->clock);

            f64 current_time = app_state->clock.elapsed;

            f64 delta = (current_time - app_state->last_time);

            f64 frame_start_time = platform_get_absolute_time();

            if (!app_state->game_inst->update(app_state->game_inst, (f32)delta)) {
                KFATAL("Game update failed. Shutting down!");

                app_state->is_running = False;

                break;
            }

            // Call game render routine
            if (!app_state->game_inst->render(app_state->game_inst, (f32)delta)) {
                KFATAL("Game render failed. Shutting down!");

                app_state->is_running = False;

                break;
            }

            // TODO: refactor packet creation
            render_packet packet;

            packet.delta_time = delta;

            renderer_draw_frame(&packet);

            // Figure out how long the frame took and, if below
            f64 frame_end_time = platform_get_absolute_time();

            f64 frame_elapsed_time = frame_end_time - frame_start_time;

            running_time += frame_elapsed_time;

            f64 remaining_seconds = target_frame_seconds - frame_elapsed_time;

            if (remaining_seconds > 0) {
                u64 remaining_ms = (remaining_seconds * 1000);

                // If there is time left, give it back to the OS.
                b8 limit_frames = False;

                if (remaining_ms > 0 && limit_frames) {
                    platform_sleep(remaining_ms - 1);
                }

                frame_count++;
            }

            // NOTE: Input update/state copying should always be handled
            // after any input should be recorded; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
            input_update(delta);

            // Update last time
            app_state->last_time = current_time;
        }
    }

    // Ensure running state is cleared
    app_state->is_running = False;

    // Shutdown event system.
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_unregister(EVENT_CODE_RESIZED, 0, application_on_resized);

    input_system_shutdown(app_state->input_system_state);
    renderer_system_shutdown(app_state->renderer_system_state);

    // Clean up platform resources
    platform_system_shutdown(app_state->platform_system_state);

    shutdown_memory(app_state->memory_system_state);

    event_system_shutdown(app_state->event_system_state);

    return True;
}

void application_get_framebuffer_size(u32* width, u32* height) {
    *width = app_state->width;
    *height = app_state->height;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            KINFO("EVENT_CODE_APPLICATION_QUIT received, shutting down.\n");
            app_state->is_running = False;
            return True;
        }
    }

    return False;
}

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            // Block anything else from processing this.
            return True;
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
    return False;
}

b8 application_on_resized(u16 code, void* sender, void* listener_inst, event_context context) {
    if (code == EVENT_CODE_RESIZED) {
        u16 width = context.data.u16[0];
        u16 height = context.data.u16[1];
        b8 is_visible = (b8)context.data.u16[2];  // Get visibility flag

        // Always update stored dimensions
        app_state->width = width;
        app_state->height = height;

        // Determine if we should suspend
        b8 should_suspend = !is_visible || (width <= 10 || height <= 10);

        if (should_suspend && !app_state->is_suspended) {
            KINFO("Window minimized or hidden, suspending application.");
            app_state->is_suspended = True;
        } else if (!should_suspend && app_state->is_suspended) {
            KINFO("Window restored, resuming application.");
            app_state->is_suspended = False;

            // Only trigger resize if we have valid dimensions
            if (width > 10 && height > 10) {
                app_state->game_inst->on_resize(app_state->game_inst, width, height);
                renderer_on_resized(width, height);
            }
        } else if (!should_suspend) {
            // Normal resize case
            app_state->game_inst->on_resize(app_state->game_inst, width, height);
            renderer_on_resized(width, height);
        }

        return True;
    }
    return False;
}

//