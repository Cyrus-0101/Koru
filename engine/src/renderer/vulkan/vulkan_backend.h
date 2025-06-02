#pragma once

#include "renderer/renderer_backend.h"

/**
 * @file vulkan_backend.h
 * @brief Public interface for the Vulkan renderer backend implementation.
 *
 * This module implements the function table required by the renderer_frontend,
 * providing Vulkan-specific implementations for initialization, shutdown,
 * frame handling, and window resize handling.
 */

/**
 * @brief Initializes the Vulkan renderer backend.
 *
 * Creates the Vulkan instance and prepares internal resources needed for rendering.
 *
 * @param backend A pointer to the renderer backend structure.
 * @param application_name Name of the application using the renderer.
 * @param plat_state A pointer to the platform-specific state.
 * @return TRUE if successful; otherwise FALSE.
 */
b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, struct platform_state* plat_state);

/**
 * @brief Shuts down the Vulkan renderer backend.
 *
 * Cleans up all resources allocated during initialization.
 *
 * @param backend A pointer to the renderer backend structure.
 */
void vulkan_renderer_backend_shutdown(renderer_backend* backend);

/**
 * @brief Handles a window resize event for the Vulkan backend.
 *
 * May involve recreating the swapchain or adjusting viewport settings.
 *
 * @param backend A pointer to the renderer backend structure.
 * @param width New width of the window.
 * @param height New height of the window.
 */
void vulkan_renderer_backend_on_resized(renderer_backend* backend, u16 width, u16 height);

/**
 * @brief Begins a new rendering frame in the Vulkan backend.
 *
 * Sets up command buffers, render passes, etc.
 *
 * @param backend A pointer to the renderer backend structure.
 * @param delta_time Time in seconds since the last frame.
 * @return TRUE if successful; otherwise FALSE.
 */
b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 delta_time);

/**
 * @brief Ends the current rendering frame in the Vulkan backend.
 *
 * Submits command buffers, presents the image to the screen, and cleans up per-frame resources.
 *
 * @param backend A pointer to the renderer backend structure.
 * @param delta_time Time in seconds since the last frame.
 * @return TRUE if successful; otherwise FALSE.
 */
b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 delta_time);