#include "game.h"

#include <entry.h>

#include <core/kmemory.h>

/**
 * @file entry.c
 * @brief Testbed application entry point implementation.
 *
 * This file implements the required `create_game()` function used by the engine.
 * It sets up the game configuration and allocates the game state.
 *
 * Responsibilities:
 * - Configure window settings
 * - Assign game lifecycle function pointers
 * - Allocate game-specific memory
 */

b8 create_game(game* out_game) {
    // Application configuration.
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 1280;
    out_game->app_config.start_height = 720;
    out_game->app_config.name = "Koru Engine Testbed";

    // Assign function pointers
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->initialize = game_initialize;
    out_game->on_resize = game_on_resize;

    // Create the game state.
    out_game->state = kallocate(sizeof(game_state), MEMORY_TAG_GAME);

    return True;
}