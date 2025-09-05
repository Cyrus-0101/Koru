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
    /** Pointer to backend-specific internal data (e.g., Vulkan image and view). */
    void* internal_data;
} texture;