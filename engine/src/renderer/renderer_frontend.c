#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "core/logger.h"
#include "core/kmemory.h"
#include "math/kmath.h"

#include "systems/texture_system.h"
#include "systems/material_system.h"

#include "resources/resource_types.h"

// TO-DO: Temporary Code/Imports to be moved to resource loader
#include "core/kstring.h"
#include "core/event.h"

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

    /**
     * @brief The projection matrix used for rendering.
     */
    mat4 projection;

    /**
     * @brief The view matrix used for rendering.
     */
    mat4 view;

    /**
     * @brief Near clipping plane distance.
     */
    f32 near_clip;

    /**
     * @brief Far clipping plane distance.
     */
    f32 far_clip;

    // TODO: Temporary code
    /**
     * @brief A pointer to a test material for demonstration purposes.
     */
    material* test_material;
    // End of temporary code
} renderer_system_state;

// Global pointer to the renderer backend instance
static renderer_system_state* state_ptr;

/**
 * @brief Event handler for debug events to cycle through test textures.
 *
 * This function is a temporary event handler that listens for a specific debug event
 * and cycles through a set of predefined textures for testing purposes.
 *
 * @param code The event code.
 * @param sender Pointer to the event sender.
 * @param listener_inst Pointer to the listener instance (should be the renderer state).
 * @param data Event context data (not used here).
 * @return True if the event was handled; otherwise False.
 */
b8 event_on_debug_event(u16 code, void* sender, void* listener_inst, event_context data) {
    const char* names[3] = {
        "cobblestone",
        "paving",
        "paving2"};

    static i8 choice = 2;

    // Save off the old name.
    const char* old_name = names[choice];

    choice++;
    choice %= 3;

    // Acquire the new texture.
    state_ptr->test_material->diffuse_map.texture = texture_system_acquire(names[choice], True);

    if (!state_ptr->test_material->diffuse_map.texture) {
        KWARN("event_on_debug_event no texture! Using default");
        state_ptr->test_material->diffuse_map.texture = texture_system_get_default_texture();
    }

    // Release the old texture.
    texture_system_release(old_name);

    return True;
}
// TO-DO: End of Temporary Code

b8 renderer_system_initialize(u64* memory_requirement, void* state, const char* application_name) {
    *memory_requirement = sizeof(renderer_system_state);
    if (state == 0) {
        return True;  // No state provided, nothing to initialize
    }

    state_ptr = state;

    // TO-DO: Temporary Code
    event_register(EVENT_CODE_DEBUG0, state_ptr, event_on_debug_event);
    // TO-DO: End of Temporary Code

    // TODO: Make backend type configurable via config or runtime settings
    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, &state_ptr->backend);
    // Initialize the frame counter
    state_ptr->backend.frame_number = 0;

    // Call the backend's initialization routine
    if (!state_ptr->backend.initialize(&state_ptr->backend, application_name)) {
        KFATAL("Renderer backend failed to initialize. Shutting down.");
        return False;
    }

    state_ptr->near_clip = 0.1f;
    state_ptr->far_clip = 1000.0f;
    state_ptr->projection = mat4_perspective(deg_to_rad(45.0f), 1280 / 720.0f, state_ptr->near_clip, state_ptr->far_clip);

    state_ptr->view = mat4_translation((vec3){0, 0, -30.0f});  //-30.0f
    state_ptr->view = mat4_inverse(state_ptr->view);

    return True;
}

void renderer_system_shutdown(void* state) {
    if (state_ptr) {
        // TO-DO: Temporary Code
        event_unregister(EVENT_CODE_DEBUG0, state_ptr, event_on_debug_event);
        // TO-DO: End of Temporary Code

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
        state_ptr->projection = mat4_perspective(deg_to_rad(45.0f), width / (f32)height, state_ptr->near_clip, state_ptr->far_clip);
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
        state_ptr->backend.update_global_state(state_ptr->projection, state_ptr->view, vec3_zero(), vec4_one(), 0);

        mat4 model = mat4_translation((vec3){0, 0, 0});
        // static f32 angle = 0.01f;
        // angle += 0.001f;
        // quat rotation = quat_from_axis_angle(vec3_forward(), angle, False);
        // mat4 model = quat_to_rotation_matrix(rotation, vec3_zero());

        geometry_render_data data = {};
        data.model = model;

        // TODO: Temporary Code
        // Create default material if it doesn't exist
        if (!state_ptr->test_material) {
            // Auto configure the material
            state_ptr->test_material = material_system_acquire("test_material");

            if (!state_ptr->test_material) {
                material_config config;
                string_ncopy(config.name, "test_material", MATERIAL_NAME_MAX_LENGTH);
                config.auto_release = False;
                config.diffuse_color = vec4_one(); // White
                string_ncopy(config.diffuse_map_name, DEFAULT_TEXTURE_NAME, TEXTURE_NAME_MAX_LENGTH);
                state_ptr->test_material = material_system_acquire_from_config(config);
            }
        }

        data.material = state_ptr->test_material;

        state_ptr->backend.update_object(data);

        // End the frame. If this fails, it is likely unrecoverable.
        b8 result = renderer_end_frame(packet->delta_time);

        if (!result) {
            KERROR("renderer_end_frame failed. Application shutting down...");
            return False;
        }
    }

    return True;
}

/**
 * @brief Sets the current view matrix for rendering.
 *
 * This function updates the view matrix used in the rendering pipeline.
 *
 * @param view The new view matrix to set.
 */
void renderer_set_view(mat4 view) {
    state_ptr->view = view;
}

void renderer_create_texture(
    const char* name,
    i32 width,
    i32 height,
    i32 channel_count,
    const u8* pixels,
    b8 has_transparency,
    struct texture* out_texture) {
    state_ptr->backend.create_texture(name, width, height, channel_count, pixels, has_transparency, out_texture);
}

void renderer_destroy_texture(struct texture* texture) {
    state_ptr->backend.destroy_texture(texture);
}

b8 renderer_create_material(struct material* material) {
    return state_ptr->backend.create_material(material);
}
void renderer_destroy_material(struct material* material) {
    state_ptr->backend.destroy_material(material);
}