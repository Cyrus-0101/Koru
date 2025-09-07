#include "renderer_frontend.h"
#include "renderer_backend.h"

#include "core/logger.h"
#include "core/kmemory.h"
#include "math/kmath.h"

#include "resources/resource_types.h"

// TO-DO: Temporary Code/Imports to be moved to resource loader
#include "core/kstring.h"
#include "core/event.h"

#define STB_IMAGE_IMPLEMENTATION  // Enable stb_image implementation
#include "vendor/stb_image.h"
// End of temporary code

#define TEX_DIMENSIONS 256  // Texture is 256x256

#define CHANNELS 4  ///< Bits per pixel RGBA

#define PIXEL_COUNT TEX_DIMENSIONS* TEX_DIMENSIONS  ///< Total number of pixels in the texture

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

    /**
     * @brief The texture used.
     */
    texture default_texture;

    // Temporary code
    /**
     * @brief A test texture.
     */
    texture test_diffuse;
    // End of temporary code
} renderer_system_state;

// Global pointer to the renderer backend instance
static renderer_system_state* state_ptr;

/**
 * @brief Initializes a texture structure to default values.
 *
 * This function sets the fields of the provided texture structure to
 * safe default values, ensuring it is in a known state before use.
 *
 * @param t Pointer to the texture structure to initialize.
 */
void create_texture(texture* t) {
    kzero_memory(t, sizeof(texture));

    t->generation = INVALID_ID;
}

/**
 * @brief Loads a texture from file and uploads it to the GPU.
 *
 * This function uses stb_image to load image data from disk, creates a texture
 * resource, and uploads it to the GPU via the renderer backend.
 * @param texture_name Name of the texture file (without extension).
 * @param t Pointer to the texture structure to be filled out.
 * @return True if the texture was loaded and created successfully; otherwise False.
 */
b8 load_texture(const char* texture_name, texture* t) {
    // TO-DO: Should be able to be located anywhere
    char* format_st = "assets/textures/%s.%s";  // File_name.extension
    const i32 required_channel_count = 4;
    stbi_set_flip_vertically_on_load(True);
    char full_file_path[512];

    // TO-DO: Try different extensions (compatible)
    string_format(full_file_path, format_st, texture_name, "png");

    // Use a temporary local texture to load state
    texture temp_texture;

    // Open the file and load the data
    u8* data = stbi_load(
        full_file_path,
        (i32*)&temp_texture.width,
        (i32*)&temp_texture.height,
        (i32*)&temp_texture.channel_count,
        required_channel_count);

    // Overwrite channel count to required_channel_count
    temp_texture.channel_count = required_channel_count;

    if (data) {
        KINFO("Loaded texture: %s (%i x %i x %i)", full_file_path, temp_texture.width, temp_texture.height, temp_texture.channel_count);

        u32 current_generation = t->generation;
        t->generation = INVALID_ID;  // Mark as invalid while updating

        u64 total_size = temp_texture.width * temp_texture.height * temp_texture.channel_count;
        // Check for transparency
        b32 has_transparency = False;
        for (u64 i = 0; i < total_size; i += required_channel_count) {
            if (data[i + 3] < 255) {
                has_transparency = True;
                break;
            }
        }

        // Returns 0 on failure
        if (stbi_failure_reason()) {
            KWARN("Failed to load texture %s: %s", full_file_path, stbi_failure_reason());
        }

        // Acquire internal texture data and upload to GPU
        // TO-DO: Make auto_release configurable
        renderer_create_texture(
            texture_name,
            True,
            temp_texture.width,
            temp_texture.height,
            temp_texture.channel_count,
            data,
            has_transparency,
            &temp_texture);

        // Take a copy of the old texture
        texture old = *t;

        // Assign the temp texture to the pointer
        *t = temp_texture;

        // Destroy the old texture if valid
        renderer_destroy_texture(&old);

        if (current_generation == INVALID_ID) {
            t->generation = 0;
        } else {
            t->generation = current_generation + 1;
        }

        // CLean up data
        stbi_image_free(data);

        return True;
    } else {
        if (stbi_failure_reason()) {
            KWARN("Failed to load texture %s: %s", full_file_path, stbi_failure_reason());
        }

        return False;
    }
}

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
    choice++;
    choice %= 3;

    // Load up the new texture.
    load_texture(names[choice], &state_ptr->test_diffuse);

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

    // Take a pointer to default textures for use in the backend
    state_ptr->backend.default_diffuse = &state_ptr->default_texture;

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

    // NOTE: Create default texture, a 256x256 blue/white checkerboard pattern.
    // This is done in code to eliminate asset dependencies.
    KTRACE("Creating default texture...");

    u8 pixels[PIXEL_COUNT * CHANNELS];
    // u8* pixels = kallocate(sizeof(u8) * pixel_count * bpp, MEMORY_TAG_TEXTURE);

    kset_memory(pixels, 255, sizeof(u8) * PIXEL_COUNT * CHANNELS);

    // Each pixel.
    for (u64 row = 0; row < TEX_DIMENSIONS; ++row) {
        for (u64 col = 0; col < TEX_DIMENSIONS; ++col) {
            u64 index = (row * TEX_DIMENSIONS) + col;
            u64 index_bpp = index * CHANNELS;

            if (row % 2) {
                if (col % 2) {
                    pixels[index_bpp + 0] = 0;
                    pixels[index_bpp + 1] = 0;
                }
            } else {
                if (!(col % 2)) {
                    pixels[index_bpp + 0] = 0;
                    pixels[index_bpp + 1] = 0;
                }
            }
        }
    }

    renderer_create_texture(
        "default",
        False,
        TEX_DIMENSIONS,
        TEX_DIMENSIONS,
        4,
        pixels,
        False,
        &state_ptr->default_texture);

    // Manual set the texture generation to INVALID_ID since this is a default texture
    state_ptr->default_texture.generation = INVALID_ID;

    //  TO-DO: Load other textures
    create_texture(&state_ptr->test_diffuse);

    return True;
}

void renderer_system_shutdown(void* state) {
    if (state_ptr) {
        // TO-DO: Temporary Code
        event_unregister(EVENT_CODE_DEBUG0, state_ptr, event_on_debug_event);
        // TO-DO: End of Temporary Code

        renderer_destroy_texture(&state_ptr->default_texture);

        renderer_destroy_texture(&state_ptr->test_diffuse);

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
        data.object_id = 0;  /// TO-DO: Add actual object ID
        data.model = model;
        data.textures[0] = &state_ptr->test_diffuse;  //

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
    b8 auto_release,
    i32 width,
    i32 height,
    i32 channel_count,
    const u8* pixels,
    b8 has_transparency,
    struct texture* out_texture) {
    state_ptr->backend.create_texture(name, auto_release, width, height, channel_count, pixels, has_transparency, out_texture);
}

void renderer_destroy_texture(struct texture* texture) {
    state_ptr->backend.destroy_texture(texture);
}