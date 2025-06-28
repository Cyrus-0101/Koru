#pragma once

#include "defines.h"

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
    b8 (*initialize)(struct renderer_backend* backend, const char* application_name, struct platform_state* plat_state);

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
     * @brief Ends the current rendering frame.
     *
     * @param backend A pointer to the renderer backend instance.
     * @param delta_time Time in seconds since the last frame.
     * @return True if successful; otherwise False.
     */
    b8 (*end_frame)(struct renderer_backend* backend, f32 delta_time);
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