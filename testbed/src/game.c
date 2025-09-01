#include "game.h"

#include "core/input.h"
#include "core/kmemory.h"
#include <core/logger.h>

#include <math/kmath.h>

// TO-DO: (Remove) This should never be available outside the engine
#include <renderer/renderer_frontend.h>

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
 *
 */
void recalculate_view_matrix(game_state* state) {
    if (state->camera_view_dirty) {
        mat4 rotation = mat4_euler_xyz(state->camera_euler.x, state->camera_euler.y, state->camera_euler.z);
        mat4 translation = mat4_translation(state->camera_position);

        state->view = mat4_mul(rotation, translation);
        state->view = mat4_inverse(state->view);

        state->camera_view_dirty = False;
    }
}

void camera_pitch(game_state* state, f32 amount) {
    state->camera_euler.x += amount;

    // Clamp to avoid Gimball lock
    f32 limit = deg_to_rad(89.0f);
    state->camera_euler.x = KCLAMP(state->camera_euler.x, -limit, limit);

    state->camera_view_dirty = True;
}

void camera_yaw(game_state* state, f32 amount) {
    state->camera_euler.y += amount;
    state->camera_view_dirty = True;
}

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

    game_state* state = (game_state*)game_inst->state;

    state->camera_position = (vec3){0, 0, 30.0f};
    state->camera_euler = vec3_zero();

    state->view = mat4_translation(state->camera_position);
    state->view = mat4_inverse(state->view);
    state->camera_view_dirty = True;

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
    game_state* state = (game_state*)game_inst->state;

    // HACKY WAY OF MOVING CAMERA AROUND
    if (input_is_key_down(KEY_UP)) {
        camera_pitch(state, 1.0f * delta_time);
    }

    if (input_is_key_down('A') || input_is_key_down(KEY_LEFT)) {
        camera_yaw(state, 1.0f * delta_time);
    }

    if (input_is_key_down(KEY_DOWN)) {
        camera_pitch(state, -1.0f * delta_time);
    }

    if (input_is_key_down('D') || input_is_key_down(KEY_RIGHT)) {
        camera_yaw(state, -1.0f * delta_time);
    }

    f32 temp_move_speed = 50.0f;
    vec3 velocity = vec3_zero();

    if (input_is_key_down('W')) {
        vec3 forward = mat4_forward(state->view);
        velocity = vec3_add(velocity, forward);
    }

    if (input_is_key_down('S')) {
        vec3 backward = mat4_backward(state->view);
        velocity = vec3_add(velocity, backward);
    }

    if (input_is_key_down('Q')) {
        vec3 left = mat4_left(state->view);
        velocity = vec3_add(velocity, left);
    }

    if (input_is_key_down('E')) {
        vec3 right = mat4_right(state->view);
        velocity = vec3_add(velocity, right);
    }

    if (input_is_key_down(KEY_SPACE)) {
        velocity.y += 1.0f;
    }

    if (input_is_key_down('X')) {
        velocity.y -= 1.0f;
    }

    vec3 z = vec3_zero();
    if (!vec3_compare(z, velocity, 0.0002f)) {
        // Normalize the volocity before applying speed
        vec3_normalize(&velocity);
        state->camera_position.x += velocity.x * temp_move_speed * delta_time;
        state->camera_position.y += velocity.y * temp_move_speed * delta_time;
        state->camera_position.z += velocity.z * temp_move_speed * delta_time;

        state->camera_view_dirty = True;
    }

    recalculate_view_matrix(state);

    // HACKY EXPOSE TO BE REMOVED. Should not be exposed outside engine.
    renderer_set_view(state->view);

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