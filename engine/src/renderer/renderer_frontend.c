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

// Forward declaration for platform state
struct platform_state;

/**
 * @brief A pointer to the active renderer backend instance.
 *
 * This is set during initialization and used throughout the lifetime of the application
 * to dispatch rendering commands to the appropriate backend.
 */
static renderer_backend* backend = 0;

/**
 * @brief Initializes the rendering system.
 *
 * Creates the appropriate rendering backend (currently hardcoded to Vulkan),
 * and calls its initialize function to prepare for rendering.
 *
 * @param application_name Name of the application using the renderer.
 * @param plat_state A pointer to the platform-specific state.
 * @return TRUE if initialization was successful; otherwise FALSE.
 */
b8 renderer_initialize(const char* application_name, struct platform_state* plat_state) {
    // Allocate memory for the backend structure
    backend = kallocate(sizeof(renderer_backend), MEMORY_TAG_RENDERER);

    // TODO: Make backend type configurable via config or runtime settings
    if (!renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_state, backend)) {
        KFATAL("Failed to create renderer backend.");
        return FALSE;
    }

    // Initialize the frame counter
    backend->frame_number = 0;

    // Call the backend's initialization routine
    if (!backend->initialize(backend, application_name, plat_state)) {
        KFATAL("Renderer backend failed to initialize. Shutting down.");
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief Shuts down the rendering system.
 *
 * Calls the backend's shutdown routine and frees allocated resources.
 */
void renderer_shutdown() {
    // Call the backend's shutdown function
    backend->shutdown(backend);

    // Free the backend memory
    kfree(backend, sizeof(renderer_backend), MEMORY_TAG_RENDERER);
}

/**
 * @brief Begins a new rendering frame.
 *
 * This typically involves acquiring a swapchain image, setting up command buffers,
 * and preparing for rendering operations.
 *
 * @param delta_time Time in seconds since the last frame.
 * @return TRUE if the frame was successfully started; otherwise FALSE.
 */
b8 renderer_begin_frame(f32 delta_time) {
    return backend->begin_frame(backend, delta_time);
}

/**
 * @brief Ends the current rendering frame.
 *
 * This usually involves submitting command buffers, presenting the rendered image,
 * and cleaning up per-frame resources.
 *
 * @param delta_time Time in seconds since the last frame.
 * @return TRUE if the frame was successfully ended; otherwise FALSE.
 */
b8 renderer_end_frame(f32 delta_time) {
    b8 result = backend->end_frame(backend, delta_time);
    backend->frame_number++; // Increment frame number after end of frame
    return result;
}

/**
 * @brief Performs the complete frame rendering process.
 *
 * Begins the frame, executes draw commands (not yet implemented here),
 * and ends the frame. Called once per frame by the engine.
 *
 * @param packet A pointer to the render packet containing frame-specific data.
 * @return TRUE if the frame was drawn successfully; otherwise FALSE.
 */
b8 renderer_draw_frame(render_packet* packet) {
    // Begin the frame
    if (renderer_begin_frame(packet->delta_time)) {
        // Perform mid-frame operations (will be expanded later)

        // End the frame
        b8 result = renderer_end_frame(packet->delta_time);

        if (!result) {
            KERROR("renderer_end_frame failed. Application shutting down...");
            return FALSE;
        }
    }

    return TRUE;
}