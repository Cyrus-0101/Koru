#pragma once

#include <defines.h>
#include <game_types.h>

/**
 * @file game.h
 * @brief Interface for the testbed game implementation.
 *
 * Declares the game lifecycle functions expected by the engine:
 * - `game_initialize()`
 * - `game_update()`
 * - `game_render()`
 * - `game_on_resize()`
 *
 * Also defines the `game_state` struct for storing per-frame data.
 */

/**
 * @brief Internal game state structure.
 *
 * This struct holds data that persists across frames.
 * Currently only stores delta time, but can be expanded
 * to include player position, world state, etc.
 */
typedef struct game_state {
    /**
     * @brief Time in seconds since the last frame.
     */
    f32 delta_time;
} game_state;

/**
 * @brief Called when the game instance is initialized.
 *
 * Used to set up systems like rendering, input, audio, etc.
 *
 * @param game_inst A pointer to the current game instance.
 * @return True if initialization was successful; False otherwise.
 */
b8 game_initialize(game* game_inst);

/**
 * @brief Called every frame to update game logic.
 *
 * Handles input, physics, AI, etc.
 *
 * @param game_inst A pointer to the current game instance.
 * @param delta_time Time in seconds since the last frame.
 * @return True if update succeeded; False to quit the application.
 */
b8 game_update(game* game_inst, f32 delta_time);

/**
 * @brief Called every frame to render the game.
 *
 * Handles drawing to screen using a renderer or graphics API.
 *
 * @param game_inst A pointer to the current game instance.
 * @param delta_time Time in seconds since the last frame.
 * @return True if rendering succeeded; False on unrecoverable error.
 */
b8 game_render(game* game_inst, f32 delta_time);

/**
 * @brief Called when the window is resized.
 *
 * Should be used to adjust viewport, projection matrices, UI layout, etc.
 *
 * @param game_inst A pointer to the current game instance.
 * @param width The new width of the window client area.
 * @param height The new height of the window client area.
 */
void game_on_resize(game* game_inst, u32 width, u32 height);