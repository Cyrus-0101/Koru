#pragma once

#include "resources/resource_types.h"

/**
 * @file resource_system.h
 *
 * @brief Resource management system.
 *
 * This module provides functionality to manage various types of resources within the engine.
 * It supports loading, acquiring, and unloading resources, as well as registering custom resource loaders.
 *
 * Usage:
 * - Initialize the resource system with `resource_system_initialize()`, providing configuration options.
 * - Register resource loaders for different resource types using `resource_system_register_loader()`.
 * - Load resources by name and type using `resource_system_load()` or `resource_system_load_custom()`.
 * - Unload resources using `resource_system_unload()`.
 * - Shutdown the resource system with `resource_system_shutdown()` to free all resources.
 */

/**
 * @struct resource_system_config
 *
 * @brief Configuration structure for initializing the resource system.
 */
typedef struct resource_system_config {
    /** Maximum number of resources that can be loaded at once. Default 32 */
    u32 max_loader_count;
    /** The relative base path for assets. */
    char* asset_base_path;
} resource_system_config;

/**
 * @struct resource_loader
 *
 * @brief Structure representing a resource loader.
 *
 * Contains information about the loader, including its type, loading and unloading functions.
 */
typedef struct resource_loader {
    /** Unique identifier for the resource loader. */
    u32 id;
    /** Type of resource this loader handles. */
    resource_type type;
    /** Custom type string for user-defined resource types. */
    const char* custom_type;
    /** Base path where resources of this type are located. */
    const char* type_path;
    /**
     * @brief Function pointer to the load function for this resource type.
     *
     * @param self Pointer to the resource_loader instance.
     * @param name Name of the resource to load.
     * @param out_resource Pointer to the resource structure to populate with loaded data.
     * @return True if loading was successful, False otherwise.
     */
    b8 (*load)(struct resource_loader* self, const char* name, resource* out_resource);
    /**
     * @brief Function pointer to the unload function for this resource type.
     *
     * @param self Pointer to the resource_loader instance.
     * @param resource Pointer to the resource to unload.
     */
    void (*unload)(struct resource_loader* self, resource* resource);
} resource_loader;

/**
 * @brief Initializes the resource system with the provided configuration.
 *
 * @param memory_requirement Pointer to a variable that will be set to the required memory size for the system.
 * @param state Pointer to a pre-allocated block of memory for the system state. If nullptr, the function will only calculate the memory requirement.
 * @param config Configuration parameters for initializing the system.
 * @return True if initialization was successful, False otherwise.
 */
b8 resource_system_initialize(u64* memory_requirement, void* state, resource_system_config config);

/**
 * @brief Shuts down the resource system and releases any allocated resources.
 *
 * @param state Pointer to the system state to be shut down.
 */
void resource_system_shutdown(void* state);

/**
 * @brief Registers a resource loader with the resource system.
 *
 * @param loader The resource_loader instance to register.
 * @return True if registration was successful, False otherwise.
 */
KAPI b8 resource_system_register_loader(resource_loader loader);

/**
 * @brief Loads a resource by name and type using the appropriate registered loader.
 *
 * @param name Name of the resource to load.
 * @param type Type of the resource to load.
 * @param out_resource Pointer to the resource structure to populate with loaded data.
 * @return True if loading was successful, False otherwise.
 */
KAPI b8 resource_system_load(const char* name, resource_type type, resource* out_resource);

/**
 * @brief Loads a custom resource by name and custom type using the appropriate registered loader.
 *
 * @param name Name of the resource to load.
 * @param custom_type Custom type string of the resource to load.
 * @param out_resource Pointer to the resource structure to populate with loaded data.
 * @return True if loading was successful, False otherwise.
 */
KAPI b8 resource_system_load_custom(const char* name, const char* custom_type, resource* out_resource);

/**
 * @brief Unloads a previously loaded resource, freeing its associated data.
 *
 * @param resource Pointer to the resource to unload.
 */
KAPI void resource_system_unload(resource* resource);

/**
 * @brief Retrieves the base path used for asset loading.
 *
 * @return The base path as a string.
 */
KAPI const char* resource_system_base_path();
