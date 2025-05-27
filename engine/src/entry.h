#pragma once

#include "core/application.h"
#include "core/kmemory.h"
#include "core/logger.h"
#include "game_types.h"

/**
 * @file entry.h
 * @brief Main entry point interface for the engine.
 *
 * This file defines the externally-defined `create_game()` function that must be implemented by client applications.
 * It also contains the `main()` function declaration which initializes and runs the game loop.
 *
 * Responsibilities:
 * - Define the entry point contract
 * - Ensure proper initialization order
 * - Integrate memory and logging systems at startup
 */

/**
 * @brief Externally-defined function to create and configure a game instance.
 *
 * This function must be implemented by the client application (e.g., testbed).
 * It sets up:
 * - Window configuration
 * - Game-specific function pointers (initialize/update/render/etc.)
 * - Game state memory allocation
 *
 * @param out_game A pointer to an uninitialized game struct that will be populated.
 * @return TRUE if the game was successfully created; FALSE otherwise.
 */
extern b8 create_game(game* out_game);

/**
 * @brief The main entry point of the application.
 *
 * This function performs the following steps:
 * 1. Requests a configured game instance from create_game()
 * 2. Validates required function pointers
 * 3. Initializes the core application system
 * 4. Starts the main application/game loop
 *
 * @return Exit code indicating success or failure.
 *         - 0: Success
 *         - -1: Failed to create game
 *         - -2: Missing required game function pointers
 *         - 1: Application failed during initialization
 *         - 2: Application failed during shutdown
 */
int main(void) {
    initialize_memory();

    // Request the game instance from the application.
    game game_inst;

    // Attempt to create the game instance
    if (!create_game(&game_inst)) {
        KFATAL("Could not create game!");
        return -1;
    }

    // Ensure the function pointers are assigned/exist.
    if (!game_inst.render || !game_inst.update || !game_inst.initialize || !game_inst.on_resize) {
        KFATAL("The game's function pointers must be assigned!");
        return -2;
    }

    // Initialize the application with the provided game configuration
    if (!application_create(&game_inst)) {
        KINFO("Application failed to create!.");
        return 1;
    }

    // Start the main application loop
    if (!application_run()) {
        KINFO("Application did not shutdown gracefully.");
        return 2;
    }

    shutdown_memory();

    return 0;
}