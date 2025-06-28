#include "game.h"

#include "core/input.h"
#include "core/kmemory.h"
#include <core/logger.h>

/**
 * @file game.c
 * @brief Implementation of the testbed game logic.
 *
 * Contains placeholder implementations for the core game functions:
 * - Initialization (`game_initialize`)
 * - Frame update (`game_update`)
 * - Rendering (`game_render`)
 * - Resize handling (`game_on_resize`)
 *
 * Can be extended to add real gameplay features.
 */

/**
 * @brief Initializes the game instance.
 *
 * Currently just logs a debug message, but should be extended to initialize:
 * - Renderer
 * - Input system
 * - Game world
 *
 * @param game_inst A pointer to the current game instance.
 * @return True if initialization succeeded; False otherwise.
 */
b8 game_initialize(game* game_inst) {
    KDEBUG("game_initialize() called!");
    return True;
}

/**
 * @brief Updates the game state each frame.
 *
 * Currently a placeholder; this would normally handle:
 * - Input processing
 * - Game logic updates
 * - Physics simulation
 *
 * @param game_inst A pointer to the current game instance.
 * @param delta_time Time in seconds since the last frame.
 * @return True if update succeeded; False to request application exit.
 */
b8 game_update(game* game_inst, f32 delta_time) {

    static u64 alloc_count = 0;
    u64 prev_alloc_count = alloc_count;
    alloc_count = get_memory_alloc_count();

    if (input_is_key_down('M') && input_was_key_up('M')) {
        KDEBUG("Memory Allocations: %llu (%llu this frame)", alloc_count, alloc_count - prev_alloc_count);
    }

    return True;
}

/**
 * @brief Renders the game each frame.
 *
 * Currently a placeholder; this would normally handle:
 * - Drawing geometry
 * - Updating shaders
 * - Rendering UI
 *
 * @param game_inst A pointer to the current game instance.
 * @param delta_time Time in seconds since the last frame.
 * @return True if rendering succeeded; False on unrecoverable error.
 */
b8 game_render(game* game_inst, f32 delta_time) {
    return True;
}

/**
 * @brief Handles window resize events.
 *
 * Currently does nothing, but should be extended to:
 * - Update viewport dimensions
 * - Recalculate aspect ratios
 * - Resize framebuffers or shadow maps
 *
 * @param game_inst A pointer to the current game instance.
 * @param width New width of the window client area.
 * @param height New height of the window client area.
 */
void game_on_resize(game* game_inst, u32 width, u32 height) {
}