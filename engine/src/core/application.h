#pragma once  // Ensures this header is only included once per compilation unit

#include "defines.h"  // Core definitions: types, asserts, platform detection, etc.

// Forward declaration of game struct
struct game;

/**
 * @brief Configuration settings for initializing the application.
 *
 * This structure contains window-related configuration such as position, size,
 * and application name. It is passed to application_create() during startup.
 *
 */
typedef struct application_config {
    /**
     * @brief The initial X position of the application window on the screen.
     */
    i16 start_pos_x;

    /**
     * @brief The initial Y position of the application window on the screen.
     */
    i16 start_pos_y;

    /**
     * @brief The initial width of the application window client area.
     */
    i16 start_width;

    /**
     * @brief The initial height of the application window client area.
     */
    i16 start_height;

    /**
     * @brief The name/title of the application shown in the window title bar.
     */
    char* name;
} application_config;

/**
 * @brief Creates and initializes the application instance.
 *
 * Sets up internal systems like memory, logging, input, and prepares the game loop.
 * Must be called before application_run().
 *
 * @param config A pointer to an application_config struct containing setup parameters.
 * @return TRUE if initialization was successful; FALSE otherwise.
 */
KAPI b8 application_create(struct game* game_inst);

/**
 * @brief Starts the main application loop.
 *
 * Begins running the game loop, which includes:
 * - Processing events
 * - Updating game logic
 * - Rendering frames
 *
 * This function blocks until the application exits.
 *
 * @return TRUE if the application exited cleanly; FALSE if an error occurred or early exit.
 */
b8 application_run();