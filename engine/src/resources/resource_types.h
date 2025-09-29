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
    TEXTURE_USE_UNKNOWN = 0x00,
    TEXTURE_USE_MAP_DIFFUSE = 0x01
} texture_use;