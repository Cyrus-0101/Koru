#pragma once

#include "renderer_types.inl"

/**
 * @file renderer_frontend.h
 * @brief Frontend interface for the rendering system.
 *
 * This module provides the main entry points for initializing, shutting down,
 * and driving the rendering system. It abstracts away the specific backend
 * (e.g., Vulkan, OpenGL) and offers a simple API to be used by the core engine.
 *
 * The frontend works with render packets and handles frame updates, resizing,
 * and drawing operations.
 */

/**
 * @struct static_mesh_data
 * @brief Forward declaration of static mesh data structure.
 *
 * Full definition is provided elsewhere. This structure represents geometry
 * data that can be rendered by the renderer.
 */
struct static_mesh_data;

/**
 * @struct platform_state
 * @brief Forward declaration of platform-specific state structure.
 *
 * Holds windowing and OS-specific information required by the renderer.
 * Defined in the platform abstraction layer.
 */
struct platform_state;

/**
 * @brief Initializes the rendering system.
 *
 * Sets up the appropriate rendering backend (e.g., Vulkan), creates necessary
 * context, device, swapchain, etc. based on application requirements.
 *
 * @param application_name Name of the application using the renderer.
 * @param plat_state A pointer to the platform-specific state.
 * @return True if initialization was successful; otherwise False.
 */
b8 renderer_system_initialize(u64* memory_requirement, void* state, const char* application_name);

/**
 * @brief Shuts down the rendering system.
 *
 * Cleans up all resources allocated during initialization, including
 * devices, command pools, buffers, and synchronization primitives.
 */
void renderer_system_shutdown(void* state);

/**
 * @brief Handles window resize events.
 *
 * Notifies the renderer that the window dimensions have changed and triggers
 * any necessary internal updates such as recreating the swapchain or adjusting
 * viewport/scissor settings.
 *
 * @param width New width of the window.
 * @param height New height of the window.
 */
void renderer_on_resized(u16 width, u16 height);

/**
 * @brief Renders a single frame using the provided packet data.
 *
 * Begins the frame, submits draw commands, and ends the frame. Called once per frame.
 *
 * @param packet A pointer to the render packet containing frame-specific data.
 * @return True if the frame was drawn successfully; otherwise False.
 */
b8 renderer_draw_frame(render_packet* packet);