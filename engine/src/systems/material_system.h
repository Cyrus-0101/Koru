#pragma once

#include "defines.h"

#include "resources/resource_types.h"

/**
 * @file material_system.h
 *
 * @brief Material management system.
 *
 * This module provides functionality to manage material resources within the engine.
 * It supports loading, acquiring, and releasing materials, as well as automatic cleanup of unused resources.
 *
 * Usage:
 * - Initialize the material system with `material_system_initialize()`, providing configuration options.
 * - Acquire materials by name using `material_system_acquire()`. If the material is not
 * already loaded, it will be created with default properties.
 * - Release materials using `material_system_release()`. If a material was marked for auto-release,
 * it will be freed when no longer in use.
 * - Shutdown the material system with `material_system_shutdown()` to free all resources.
 */

/**
 * @brief Default material name used when a requested texture is not found.
 */
#define DEFAULT_MATERIAL_NAME "default"

/**
 * @struct material_system_config
 *
 * @brief Configuration options for initializing the material system.
 */
typedef struct material_system_config {
    /** Maximum number of materials that can be managed by the system. */
    u32 max_material_count;
} material_system_config;

/**
 * @brief Initializes the material system.
 *
 * @param memory_requirement Pointer to store the required memory size for the system state.
 * @param state Pointer to pre-allocated memory for the system state.
 * @param config Configuration options for the material system.
 * @return `True` if initialization was successful, `False` otherwise.
 */
b8 material_system_initialize(u64* memory_requirement, void* state, material_system_config config);

/**
 * @brief Shuts down the material system, freeing all resources.
 *
 * @param state Pointer to the material system state.
 */
void material_system_shutdown(void* state);

/**
 * @brief Acquires a material by name.
 *
 * If the material is not already loaded, it will be created with default properties.
 *
 * @param name Name of the material to acquire.
 * @return Pointer to the acquired material, or `NULL` if acquisition failed.
 */
material* material_system_acquire(const char* name);

/**
 * @brief Acquires a material based on the provided configuration.
 *
 * If a material with the same name already exists, it will be returned.
 * Otherwise, a new material will be created using the specified configuration.
 *
 * @param config Configuration for the material to be acquired or created.
 * @return Pointer to the acquired or newly created material, or `NULL` if acquisition failed.
 */
material* material_system_acquire_from_config(material_config config);

/**
 * @brief Releases a material by name.
 *
 * If the material was marked for auto-release, it will be freed when no longer in use.
 *
 * @param name Name of the material to release.
 */
void material_system_release(const char* name);

/**
 * @brief Retrieves the default material used when a requested material is not found.
 *
 * @return Pointer to the default material.
 */
material* material_system_get_default();