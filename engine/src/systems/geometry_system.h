#pragma once

#include "renderer/renderer_types.inl"

/**
 * @file geometry_system.h
 *
 * @brief Geometry management system.
 *
 * This module provides functionality to manage geometry resources within the engine.
 * It supports loading, acquiring, and releasing geometries, as well as automatic cleanup of unused resources.
 *
 * Usage:
 * - Initialize the geometry system with `geometry_system_initialize()`, providing configuration options.
 * - Acquire geometries by name using `geometry_system_acquire()`. If the geometry is not
 * already loaded, it will be created with default properties.
 * - Release geometries using `geometry_system_release()`. If a geometry was marked for auto-release,
 * it will be freed when no longer in use.
 * - Shutdown the geometry system with `geometry_system_shutdown()` to free all resources.
 */

/**
 * @brief Default geometry name used when a requested geometry is not found.
 */
#define DEFAULT_GEOMETRY_NAME "default"

/**
 *
 * @struct geometry_system_config
 *
 * @brief Configuration structure for initializing the geometry system.
 *
 */
typedef struct geometry_system_config {
    // Max number of geometries that can be loaded at once.
    // NOTE: Should be significantly greater than the number of static meshes because
    // the there can and will be more than one of these per mesh.
    // Take other systems into account as well
    u32 max_geometry_count;
} geometry_system_config;

/**
 * @struct geometry_config
 *
 * @brief Configuration structure for creating a geometry.
 */
typedef struct geometry_config {
    /** Number of vertices in the geometry. */
    u32 vertex_count;
    /** Pointer to an array of vertices defining the geometry. */
    vertex_3d* vertices;
    /** Index count for the geometry. */
    u32 index_count;
    /** Pointer to an array of indices defining the geometry. */
    u32* indices;
    /** Name of the geometry for identification purposes. */
    char name[GEOMETRY_NAME_MAX_LENGTH];
    /** Name of the material to be associated with this geometry. */
    char material_name[MATERIAL_NAME_MAX_LENGTH];
} geometry_config;

/**
 * @brief Initializes the geometry system with the provided configuration.
 *
 * @param memory_requirement Pointer to a variable that will be set to the required memory size for the system.
 * @param state Pointer to a pre-allocated block of memory for the system state. If
 * nullptr, the function will only calculate the memory requirement.
 * @param config Configuration parameters for initializing the system.
 * @return True if initialization was successful, False otherwise.
 */
b8 geometry_system_initialize(u64* memory_requirement, void* state, geometry_system_config config);

/**
 * @brief Shuts down the geometry system and releases any allocated resources.
 *
 * @param state Pointer to the system state to be shut down.
 */
void geometry_system_shutdown(void* state);

/**
 * @brief Acquires an existing geometry by id.
 *
 * @param id The geometry identifier to acquire by.
 * @return A pointer to the acquired geometry or nullptr if failed.
 */
geometry* geometry_system_acquire_by_id(u32 id);

/**
 * @brief Registers and acquires a new geometry using the given config.
 *
 * @param config The geometry configuration.
 * @param auto_release Indicates if the acquired geometry should be unloaded when its reference count reaches 0.
 * @return A pointer to the acquired geometry or nullptr if failed.
 */
geometry* geometry_system_acquire_from_config(geometry_config config, b8 auto_release);

/**
 * @brief Releases a reference to the provided geometry.
 *
 * @param geometry The geometry to be released.
 */
void geometry_system_release(geometry* geometry);

/**
 * @brief Obtains a pointer to the default geometry.
 *
 * @return A pointer to the default geometry.
 */
geometry* geometry_system_get_default();

/**
 * @brief Generates configuration for plane geometries given the provided parameters.
 * NOTE: vertex and index arrays are dynamically allocated and should be freed upon object disposal.
 * Thus, this should not be considered production code.
 *
 * @param width The overall width of the plane. Must be non-zero.
 * @param height The overall height of the plane. Must be non-zero.
 * @param x_segment_count The number of segments along the x-axis in the plane. Must be non-zero.
 * @param y_segment_count The number of segments along the y-axis in the plane. Must be non-zero.
 * @param tile_x The number of times the texture should tile across the plane on the x-axis. Must be non-zero.
 * @param tile_y The number of times the texture should tile across the plane on the y-axis. Must be non-zero.
 * @param name The name of the generated geometry.
 * @param material_name The name of the material to be used.
 * @return A geometry configuration which can then be fed into geometry_system_acquire_from_config().
 */
geometry_config geometry_system_generate_plane_config(f32 width, f32 height, u32 x_segment_count, u32 y_segment_count, f32 tile_x, f32 tile_y, const char* name, const char* material_name);