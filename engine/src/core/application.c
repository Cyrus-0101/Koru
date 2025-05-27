#include "application.h"
#include "game_types.h"
#include "platform/platform.h"
#include "core/logger.h"

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
        KERROR("Error: application_create called more than once");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Initialize Subsystem
    initialize_logging();

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

    // Start platform layer
    if (!platform_startup(&app_state.platform, game_inst->app_config.name, game_inst->app_config.start_pos_x, game_inst->app_config.start_pos_y, game_inst->app_config.start_width, game_inst->app_config.start_height)) {
        return FALSE;
    }

    // Initialize Game
    if (!app_state.game_inst->initialize(app_state.game_inst)) {
        KFATAL("ERROR: Game failed to initialize");

        return FALSE;
    } 

    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);

    initialized = TRUE;

    return TRUE;
}

/**
 * @brief Starts the main application loop.
 *
 * Enters a loop that continuously processes OS messages/events using platform_pump_messages(),
 * and continues until the application signals it should exit.
 *
 * Once exited, it cleans up the platform layer and returns.
 *
 * @return TRUE if the application exited cleanly; FALSE if an error occurred during shutdown.
 */
b8 application_run() {
    while (app_state.is_running) {
        if (!platform_pump_messages(&app_state.platform)) {
            // If platform request to quit, stop running
            app_state.is_running = FALSE;
            break;
        }
    }

    // Ensure running state is cleared
    app_state.is_running = FALSE;

    // Clean up platform resources
    platform_shutdown(&app_state.platform);

    return TRUE;
}