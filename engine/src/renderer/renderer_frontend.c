#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "core/logger.h"
#include "core/kmemory.h"

/**
 * @file renderer_frontend.c
 * @brief Frontend implementation for the Koru rendering system.
 *
 * This file provides the implementation for the main rendering interface functions,
 * abstracting away the specific backend (e.g., Vulkan, OpenGL) from the rest of the engine.
 * It manages initialization, frame lifecycle, and drawing operations using a render packet.
 */

/**
 * @struct renderer_system_state
 *
 * @brief Represents the state of the rendering system.
 *
 * Contains the current rendering backend and its associated state.
 */
typedef struct renderer_system_state {
    /**
     * @brief Pointer to the platform-specific state.
     */
    renderer_backend backend;
} renderer_system_state;

// Global pointer to the renderer backend instance
static renderer_system_state* state_ptr;

/**
 * @brief Initializes the rendering system.
 *
 * Creates the appropriate rendering backend (currently hardcoded to Vulkan),
 * and calls its initialize function to prepare for rendering.
 *
 * @param application_name Name of the application using the renderer.
 * @param plat_state A pointer to the platform-specific state.
 * @return True if initialization was successful; otherwise False.
 */
b8 renderer_system_initialize(u64* memory_requirement, void* state, const char* application_name) {
    *memory_requirement = sizeof(renderer_system_state);
    if (state == 0) {
        return True;  // No state provided, nothing to initialize
    }

    state_ptr = state;
    // TODO: Make backend type configurable via config or runtime settings
    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, &state_ptr->backend);
    // Initialize the frame counter
    state_ptr->backend.frame_number = 0;

    // Call the backend's initialization routine
    if (!state_ptr->backend.initialize(&state_ptr->backend, application_name)) {
        KFATAL("Renderer backend failed to initialize. Shutting down.");
        return False;
    }

    return True;
}

/**
 * @brief Shuts down the rendering system.
 *
 * Calls the backend's shutdown routine and frees allocated resources.
 */
void renderer_system_shutdown(void* state) {
    if (state_ptr) {
        state_ptr->backend.shutdown(&state_ptr->backend);
    }

    state_ptr = 0;
}

/**
 * @brief Begins a new rendering frame.
 *
 * This typically involves acquiring a swapchain image, setting up command buffers,
 * and preparing for rendering operations.
 *
 * @param delta_time Time in seconds since the last frame.
 * @return True if the frame was successfully started; otherwise False.
 */
b8 renderer_begin_frame(f32 delta_time) {
    if (!state_ptr) {
        return False;
    }

    return state_ptr->backend.begin_frame(&state_ptr->backend, delta_time);
}

/**
 * @brief Ends the current rendering frame.
 *
 * This usually involves submitting command buffers, presenting the rendered image,
 * and cleaning up per-frame resources.
 *
 * @param delta_time Time in seconds since the last frame.
 * @return True if the frame was successfully ended; otherwise False.
 */
b8 renderer_end_frame(f32 delta_time) {
    if (!state_ptr) {
        return False;
    }

    b8 result = state_ptr->backend.end_frame(&state_ptr->backend, delta_time);
    state_ptr->backend.frame_number++;  // Increment frame number after end of frame
    return result;
}

void renderer_on_resized(u16 width, u16 height) {
    if (state_ptr) {
        state_ptr->backend.resized(&state_ptr->backend, width, height);
    } else {
        KWARN("Renderer backend does not exist to accept resize: %i %i", width, height);
    }
}

/**
 * @brief Performs the complete frame rendering process.
 *
 * Begins the frame, executes draw commands (not yet implemented here),
 * and ends the frame. Called once per frame by the engine.
 *
 * @param packet A pointer to the render packet containing frame-specific data.
 * @return True if the frame was drawn successfully; otherwise False.
 */
b8 renderer_draw_frame(render_packet* packet) {
    // Begin the frame
    if (renderer_begin_frame(packet->delta_time)) {
        // Perform mid-frame operations (will be expanded later)

        // End the frame
        b8 result = renderer_end_frame(packet->delta_time);

        if (!result) {
            KERROR("renderer_end_frame failed. Application shutting down...");
            return False;
        }
    }

    return True;
}