#pragma once

#include "math/math_types.h"

/**
 * @file resource_types.h
 * @brief Defines core resource types used in the Koru Engine.
 *
 * This file defines structures for textures, materials, meshes, and other
 * resources used in rendering and asset management. These types are fundamental
 * to how the engine represents and manipulates graphical assets.
 */

/** Maximum length for texture names. */
#define TEXTURE_NAME_MAX_LENGTH 512

/** Maximum length for material names. */
#define MATERIAL_NAME_MAX_LENGTH 256

/** Maximum length for geometry names. */
#define GEOMETRY_NAME_MAX_LENGTH 256

/**
 * @struct texture
 * @brief Represents a texture resource.
 *
 * Contains metadata about the texture such as dimensions, channel count,
 * transparency, and a pointer to internal data used by the rendering backend.
 */
typedef struct texture {
    /** Unique identifier for the texture resource. */
    u32 id;
    /** Width of the texture in pixels. */
    u32 width;
    /** Height of the texture in pixels. */
    u32 height;
    /** Number of color channels in the texture (e.g., 3 for RGB, 4 for RGBA). */
    u8 channel_count;
    /** Indicates if the texture has an alpha channel for transparency. */
    b8 has_transparency;
    /** Internal data pointer for backend-specific texture representation. */
    u32 generation;
    /** Name of the texture allocated for identification purposes. */
    char name[TEXTURE_NAME_MAX_LENGTH];
    /** Pointer to backend-specific internal data (e.g., Vulkan image and view). */
    void* internal_data;
} texture;

/**
 * @enum texture_use
 * @brief Enumerates the intended uses for a texture.
 *
 * This helps the engine understand how to apply the texture in rendering,
 * such as whether it's a diffuse map, normal map, etc.
 */
typedef enum texture_use {
    /** Unknown or unspecified texture use. */
    TEXTURE_USE_UNKNOWN = 0x00,
    /** Texture is used as a diffuse map. */
    TEXTURE_USE_MAP_DIFFUSE = 0x01
} texture_use;

typedef struct texture_map {
    /** Pointer to the associated texture resource. */
    texture* texture;
    /** Intended use of the texture (e.g., diffuse map). */
    texture_use use;
} texture_map;

/**
 * @struct material
 * @brief Represents a material resource.
 *
 * Contains properties that define how surfaces appear when rendered,
 * including color and texture references.
 */
typedef struct material {
    /** Unique identifier for the material resource. */
    u32 id;
    /** Generation counter for tracking updates to the material. */
    u32 generation;
    /** Internal identifier used by the rendering backend. */
    u32 internal_id;
    /** Identifier of the texture used for the diffuse map. */
    char name[MATERIAL_NAME_MAX_LENGTH];
    /** Diffuse color of the material (used if no texture is assigned). */
    vec4 diffuse_color;
    /** Texture map used for the diffuse component of the material. */
    texture_map diffuse_map;
} material;

/**
 * @struct material_config
 * @brief Configuration structure for creating or loading a material.
 *
 * Contains parameters needed to define a material, including its name,
 * whether it should be auto-released, its diffuse color, and the name of
 * the diffuse texture map.
 */
typedef struct material_config {
    /** Name of the material. */
    char name[MATERIAL_NAME_MAX_LENGTH];
    /** Indicates if the material should be automatically released when no longer referenced. */
    b8 auto_release;
    /** Diffuse color of the material. */
    vec4 diffuse_color;
    /** Name of the diffuse texture map. */
    char diffuse_map_name[TEXTURE_NAME_MAX_LENGTH];
} material_config;

/**
 * @struct geometry
 * @brief Represents a geometry (mesh) resource.
 *
 * Contains identifiers and references to materials used for rendering the geometry.
 */
typedef struct geometry {
    /** Unique identifier for the geometry resource. */
    u32 id;
    /** Generation counter for tracking updates to the geometry. */
    u32 generation;
    /** Internal identifier used by the rendering backend. */
    u32 internal_id;
    /** Name of the geometry allocated for identification purposes. */
    char name[GEOMETRY_NAME_MAX_LENGTH];
    /** Pointer to the associated material for rendering this geometry. */
    material* material;
} geometry;

/**
 * @enum resource_type
 * @brief Enumerates the different types of resources managed by the engine.
 *
 * This helps in categorizing and handling resources appropriately during loading,
 * processing, and rendering.
 */
typedef enum resource_type {
    /** Text resource type. (char[])*/
    RESOURCE_TYPE_TEXT,
    /** Binary resource type. (uint 8b) */
    RESOURCE_TYPE_BINARY,
    /** Image resource type. */
    RESOURCE_TYPE_IMAGE,
    /** Material resource type. */
    RESOURCE_TYPE_MATERIAL,
    /** Static mesh resource type. */
    RESOURCE_TYPE_STATIC_MESH,
    /** Custom resource type for user-defined resources. */
    RESOURCE_TYPE_CUSTOM
} resource_type;

/**
 * @struct resource
 * @brief Represents a generic resource loaded by the engine.
 *
 * Contains metadata about the resource, including its type, name,
 * file path, size, and a pointer to the actual data.
 */
typedef struct resource {
    /** Identifier of the loader that loaded this resource. */
    u32 loader_id;
    /** Type of the resource (e.g., image, material). */
    const char* name;
    /** Path to the resource file. */
    char* full_path;
    /** Size of the resource data in bytes. */
    u64 data_size;
    /** Pointer to the actual resource data. */
    void* data;
} resource;

/**
 * @struct image_resource_data
 * @brief Represents the data for an image resource.
 *
 * Contains pixel data along with metadata such as dimensions and channel count.
 */
typedef struct image_resource_data {
    /** Number of color channels in the image (e.g., 3 for RGB, 4 for RGBA). */
    u8 channel_count;
    /** Width of the image in pixels. */
    u32 width;
    /** Height of the image in pixels. */
    u32 height;
    /** Pointer to the raw pixel data of the image. */
    u8* pixels;
} image_resource_data;