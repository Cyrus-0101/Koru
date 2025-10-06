#pragma once

#include "defines.h"
#include "math/math_types.h"
#include "resources/resource_types.h"

/**
 * @file renderer_types.inl
 * @brief Internal renderer type definitions for the Koru game engine.
 *
 * This file contains shared renderer-related types used across the rendering system,
 * including backend abstraction and render packet structures.
 *
 * Note: `.inl` files are typically used for inline or shared type definitions that are included
 * in multiple source files but not compiled directly.
 */

/**
 *
 *
 * @brief Specifies the available rendering backends supported by Koru.
 */
typedef enum renderer_backend_type {
    /**
     * @brief Vulkan rendering API.
     */
    RENDERER_BACKEND_TYPE_VULKAN,

    /**
     * @brief OpenGL rendering API.
     */
    RENDERER_BACKEND_TYPE_OPENGL,

    /**
     * @brief DirectX rendering API (e.g., DirectX 11/12).
     */
    RENDERER_BACKEND_TYPE_DIRECTX
} renderer_backend_type;

/**
 *
 * @brief Represents a global uniform object used for rendering.
 *
 * NVIDIA requires a 256-byte alignment for uniform buffers.
 * Each field in this structure is aligned to 16 bytes,
 * ensuring compatibility with various graphics APIs.
 *
 * Each field is 64 bytes.
 */
typedef struct global_uniform_object {
    /**
     * @brief Projection matrix used for transforming 3D coordinates to 2D screen space.
     *
     * This matrix is typically set once per frame and used for all objects rendered in that frame.
     */
    mat4 projection;

    /**
     * @brief View matrix used for transforming world coordinates to camera space.
     *
     * This matrix is updated per frame to reflect the camera's position and orientation.
     */
    mat4 view;

    /**
     * @brief Model matrix used for transforming object coordinates to world space.
     *
     * This matrix is set per object and defines its position, rotation, and scale in the world.
     */
    mat4 m_reserved0;

    /**
     * @brief Reserved for future use or additional transformations.
     *
     * This field is currently unused but may be utilized in future updates to the rendering system.
     */
    mat4 m_reserved1;
} global_uniform_object;

// To be changed to local_uniform_object
// Potentially uploaded once per object per frame
typedef struct material_uniform_object {
    vec4 diffuse_color;  // 16 bytes

    vec4 v_reserved0;  // 16 bytes - reserved for future use

    vec4 v_reserved1;  // 16 bytes - reserved for future use

    vec4 v_reserved2;  // 16 bytes - reserved for future use
} material_uniform_object;

/**
 * @struct geometry_render_data
 * @brief Contains data required to render a single geometry object.
 *
 * This structure holds the model transformation matrix and material information
 * needed for rendering an object in the scene.
 */
typedef struct geometry_render_data {
    /** Model transformation matrix for the object. */
    mat4 model;

    /** Pointer to the material used for rendering the object. */
    material* material;
} geometry_render_data;

/**
 *
 * @brief Represents an abstract rendering backend interface.
 *
 * This structure defines function pointers to be implemented by specific rendering APIs.
 * It provides a unified way to interact with different graphics backends like Vulkan, OpenGL, or DirectX.
 */
typedef struct renderer_backend {
    /**
     * @brief A pointer to the platform-specific state.
     */
    struct platform_state* plat_state;

    /**
     * @brief The current frame number (useful for synchronization or debugging).
     */
    u64 frame_number;

    /**
     * @brief Function pointer to initialize the backend.
     *
     * @param backend A pointer to the renderer backend instance.
     * @param application_name Name of the application using the renderer.
     * @param plat_state A pointer to the platform-specific state.
     * @return True if successful; otherwise False.
     */
    b8 (*initialize)(struct renderer_backend* backend, const char* application_name);

    /**
     * @brief Function pointer to shut down the backend.
     *
     * @param backend A pointer to the renderer backend instance.
     */
    void (*shutdown)(struct renderer_backend* backend);

    /**
     * @brief Function pointer called when the window is resized.
     *
     * @param backend A pointer to the renderer backend instance.
     * @param width New width of the window.
     * @param height New height of the window.
     */
    void (*resized)(struct renderer_backend* backend, u16 width, u16 height);

    /**
     * @brief Begins a new rendering frame.
     *
     * @param backend A pointer to the renderer backend instance.
     * @param delta_time Time in seconds since the last frame.
     * @return True if successful; otherwise False.
     */
    b8 (*begin_frame)(struct renderer_backend* backend, f32 delta_time);

    /**
     * @brief Updates the global state for rendering.
     *
     * This function is called each frame to update the global uniform object
     * used for rendering, such as projection and view matrices.
     *
     * @param projection The projection matrix for the current frame.
     * @param view The view matrix for the current frame.
     * @param view_position The camera's position in world space.
     * @param ambient_colour The ambient light colour in RGBA format.
     * @param mode Rendering mode (e.g., wireframe, solid).
     */
    void (*update_global_state)(mat4 projection, mat4 view, vec3 view_position, vec4 ambient_colour, i32 mode);

    /**
     * @brief Ends the current rendering frame.
     *
     * @param backend A pointer to the renderer backend instance.
     * @param delta_time Time in seconds since the last frame.
     * @return True if successful; otherwise False.
     */
    b8 (*end_frame)(struct renderer_backend* backend, f32 delta_time);

    /**
     * @brief Updates the model matrix for a specific object.
     *
     * @param model The model matrix for the object.
     *
     * @return void
     */
    void (*update_object)(geometry_render_data data);

    /**
     * @brief Creates a texture resource from raw pixel data.
     *
     * This function uploads the provided pixel data to the GPU and creates
     * a texture object that can be used in rendering operations.
     *
     * @param name Name of the texture resource.
     * @param width Width of the texture in pixels.
     * @param height Height of the texture in pixels.
     * @param channel_count Number of color channels (e.g., 3 for RGB, 4 for RGBA).
     * @param pixels Pointer to the raw pixel data
     * @param has_transparency Whether the texture contains transparency (alpha channel).
     * @param out_texture Pointer to the texture structure to be filled out.
     */
    void (*create_texture)(
        const char* name,
        i32 width,
        i32 height,
        i32 channel_count,
        const u8* pixels,
        b8 has_transparency,
        struct texture* out_texture);

    /**
     * @brief Destroys a texture resource and frees associated GPU memory.
     * @param texture Pointer to the texture to be destroyed.
     */
    void (*destroy_texture)(struct texture* texture);

    /**
     * @brief Creates a material resource.
     *
     * This function initializes a material with the provided properties
     * and prepares it for use in rendering operations.
     *
     * @param material Pointer to the material structure to be filled out.
     * @return True if the material was created successfully; otherwise False.
     */
    b8 (*create_material)(struct material* material);

    /**
     * @brief Destroys a material resource and frees associated resources.
     *
     * @param material Pointer to the material to be destroyed.
     */
    void (*destroy_material)(struct material* material);
} renderer_backend;

/**
 * @struct render_packet
 * @brief A data structure passed into the render system during each frame.
 *
 * Contains information needed for rendering the current frame.
 */
typedef struct render_packet {
    /**
     * @brief Delta time since the last frame in seconds.
     */
    f32 delta_time;
} render_packet;