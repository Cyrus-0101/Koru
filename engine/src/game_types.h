#pragma once

#include "core/application.h"

/**
 * @brief Represents the basic game interface.
 *
 * This struct defines how the engine interacts with a specific game.
 * It includes function pointers for lifecycle management and a pointer
 * to game-specific state data.
 */
typedef struct game {
    /**
     * @brief Configuration used to create the application window.
     */
    application_config app_config;

    /**
     * @brief Function pointer to the game's initialization routine.
     *
     * Called once during startup after the platform layer is initialized.
     */
    b8 (*initialize)(struct game* game_inst);

    /**
     * @brief Function pointer to the game's update routine.
     *
     * Called once per frame to update game logic.
     */
    b8 (*update)(struct game* game_inst, f32 delta_time);

    /**
     * @brief Function pointer to the game's render routine.
     *
     * Called once per frame to draw the game.
     */
    b8 (*render)(struct game* game_inst, f32 delta_time);

    /**
     * @brief Function pointer to handle window resizing.
     *
     * Optional. Can be NULL if not needed.
     */
    void (*on_resize)(struct game* game_inst, u32 width, u32 height);

    /**
     * @brief Pointer to game-specific state data.
     *
     * Allocated and managed by the game implementation.
     */
    void* state;
} game;