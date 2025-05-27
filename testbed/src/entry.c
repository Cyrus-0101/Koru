#include "game.h"

#include <entry.h>

#include <core/kmemory.h>

/**
 * @brief Creates and configures the game instance.
 *
 * Sets up:
 * - Window configuration (position, size, title)
 * - Function pointers for update, render, initialize, and resize
 * - Game-specific internal state allocation
 *
 * @param out_game A pointer to an uninitialized game struct.
 * @return TRUE if the game was successfully created; FALSE otherwise.
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

    return TRUE;
}