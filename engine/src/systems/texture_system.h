#pragma once

#include "renderer/renderer_types.inl"

/**
 * @file texture_system.h
 *
 * @brief Texture management system.
 *
 * This module provides functionality to manage texture resources within the engine.
 * It supports loading, acquiring, and releasing textures, as well as automatic cleanup of unused resources.
 *
 * Usage:
 * - Initialize the texture system with `texture_system_initialize()`, providing configuration options.
 * - Acquire textures by name using `texture_system_acquire()`. If the texture is not
 * already loaded, it will be loaded from disk.
 * - Release textures using `texture_system_release()`. If a texture was marked for auto-release,
 * it will be freed when no longer in use.
 * - Shutdown the texture system with `texture_system_shutdown()` to free all resources.
 */

/**
 * @brief Default texture name used when a requested texture is not found.
 */
#define DEFAULT_TEXTURE_NAME "default"

/**
 * @struct texture_reference
 *
 * @brief Reference to a texture in the system.
 *
 * This structure is used to manage the lifetime and reference counting of textures.
 * It contains the handle to the texture, its reference count, and whether it should be
 * automatically released when no longer needed.
 */
typedef struct texture_reference {
    /**
     * @brief Number of references to this texture.
     */
    u64 reference_count;

    /**
     * @brief Handle to the texture in the registered textures array.
     */
    u32 handle;

    /**
     * @brief Auto-release flag.
     */
    b8 auto_release;
} texture_reference;

/**
 * @struct texture_system_config
 *
 * @brief Configuration options for initializing the texture system.
 */
typedef struct texture_system_config {
    /** Maximum number of textures that can be managed by the system. */
    u32 max_texture_count;
} texture_system_config;

/**
 * @brief Initializes the texture system.
 *
 * @param memory_requirement Pointer to store the required memory size for the texture system.
 * @param state Pointer to pre-allocated memory for the texture system state.
 * @param config Configuration options for the texture system.
 *
 * @return Returns true if initialization was successful, false otherwise.
 */
b8 texture_system_initialize(u64* memory_requirement, void* state, texture_system_config config);

/**
 * @brief Shuts down the texture system, freeing all resources.
 *
 * @param state Pointer to the texture system state to be shut down.
 */
void texture_system_shutdown(void* state);

/**
 * @brief Acquires a texture by name.
 *
 * If the texture is not already loaded, it will be loaded from disk.
 * If the texture cannot be found, the default texture will be returned.
 *
 * @param name Name of the texture to acquire.
 * @param auto_release If true, the texture will be automatically released when no longer in use.
 *
 * @return Pointer to the acquired texture, or NULL if acquisition failed.
 */
texture* texture_system_acquire(const char* name, b8 auto_release);

/**
 * @brief Releases a texture by name.
 *
 * If the texture was marked for auto-release, it will be freed when no longer in use.
 *
 * @param name Name of the texture to release.
 */
void texture_system_release(const char* name);

/**
 * @brief Retrieves the default texture.
 *
 * The default texture is used when a requested texture cannot be found.
 *
 * @return Pointer to the default texture.
 */
texture* texture_system_get_default_texture();
