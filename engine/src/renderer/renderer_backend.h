#pragma once

#include "renderer_types.inl"

/**
 * @file renderer_backend.h
 * @brief Renderer backend creation and destruction interface.
 *
 * This module provides functions to instantiate and destroy specific rendering backends
 * such as Vulkan, OpenGL, or DirectX. It works with the `renderer_backend` structure to
 * provide a unified function table-based API across different graphics APIs.
 */

/**
 *
 * @brief Forward declaration of the platform-specific state structure.
 *
 * Contains windowing and OS-specific information required by the renderer.
 * Defined in the platform abstraction layer.
 */
struct platform_state;

/**
 * @brief Creates a rendering backend of the specified type.
 *
 * Initializes function pointers and internal data for the requested backend.
 * This allows the engine to interact with different rendering APIs through a common interface.
 *
 * @param type The type of renderer backend to create (e.g., Vulkan, OpenGL, DirectX).
 * @param plat_state A pointer to the platform-specific state.
 * @param out_renderer_backend A pointer to a renderer_backend structure to be populated.
 * @return True if the backend was successfully created; otherwise False.
 */
b8 renderer_backend_create(renderer_backend_type type, renderer_backend* out_renderer_backend);

/**
 * @brief Destroys a previously created rendering backend.
 *
 * Cleans up any resources associated with the backend and resets its state.
 *
 * @param renderer_backend A pointer to the renderer backend instance to destroy.
 */
void renderer_backend_destroy(renderer_backend* renderer_backend);