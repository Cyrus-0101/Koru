#pragma once  // Ensures this header is only included once per compilation unit

#include "defines.h"  // Core definitions: types, asserts, platform detection, etc.

/**
 * @file application.h
 * @brief Public interface for the core application system.
 *
 * This module provides the main entry point and lifecycle management for the engine.
 * It handles:
 * - Application configuration (window size, position, name)
 * - Initialization of core systems (memory, logging, platform layer)
 * - The main game loop
 *
 * Usage:
 * - Define a `create_game()` function that populates a `game` struct
 * - Call `application_create()` to initialize the engine
 * - Call `application_run()` to start the game loop
 */

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
 * @param game_inst A pointer to the game instance containing setup parameters.
 * @return True if initialization was successful; False otherwise.
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
 * @return True if the application exited cleanly; False if an error occurred or early exit.
 */
b8 application_run();

/**
 * @brief Retrieves the current window (framebuffer) size.
 *
 * Used by the renderer to know what resolution to create/resize the swapchain to.
 *
 * @param width Output variable for the current width.
 * @param height Output variable for the current height.
 */
void application_get_framebuffer_size(u32* width, u32* height);