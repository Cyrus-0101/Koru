#pragma once

#include "defines.h"

/**
 * @file math_types.h
 * @brief Core mathematical types used throughout the Koru engine.
 *
 * This header defines fundamental vector and quaternion types with multiple
 * access patterns (position, color, texture coordinates). The types are
 * designed for both clarity and performance, with SIMD support where available.
 */

/**
 * @brief A 2-component vector union with multiple access patterns.
 *
 * Provides access to components via:
 * - Position (x,y)
 * - Color (r,g)
 * - Texture coordinates (s,t) or (u,v)
 * - Raw array index
 *
 * Why needed:
 * - Unified type for different interpretations of 2D data
 * - Memory-efficient (no conversion between color/position/UVs needed)
 * - Cleaner code with semantic naming (e.g., .x/.y for position vs .u/.v for UVs)
 */
typedef union vec2_u {
    /** @brief Array access for direct component manipulation */
    f32 elements[2];

    /** @brief Named component access */
    struct {
        union {
            /** @brief First component (x position, red channel, s/u texture coord) */
            f32 x, r, s, u;
        };
        union {
            /** @brief Second component (y position, green channel, t/v texture coord) */
            f32 y, g, t, v;
        };
    };
} vec2;

/**
 * @brief A 3-component vector union with multiple access patterns.
 *
 * Provides access to components via:
 * - Position (x,y,z)
 * - Color (r,g,b)
 * - Texture coordinates (s,t,p)
 * - Raw array index
 *
 * Why needed:
 * - Standard representation for 3D positions and directions
 * - Color operations without separate color structs
 * - 3D texture coordinates support
 */
typedef struct vec3_u {
    union {
        /** @brief Array access for direct component manipulation */
        f32 elements[3];

        /** @brief Named component access */
        struct {
            union {
                /** @brief First component (x position, red channel, s texture coord) */
                f32 x, r, s, u;
            };
            union {
                /** @brief Second component (y position, green channel, t texture coord) */
                f32 y, g, t, v;
            };
            union {
                /** @brief Third component (z position, blue channel, p texture coord) */
                f32 z, b, p, w;
            };
        };
    };
} vec3;

/**
 * @brief A 4-component vector union with multiple access patterns.
 *
 * Provides access to components via:
 * - Position (x,y,z,w)
 * - Color (r,g,b,a)
 * - Texture coordinates (s,t,p,q)
 * - Raw array index
 * - SIMD operations when enabled
 */
typedef union vec4_u {
    // #if defined(KUSE_SIMD)
    //     /** @brief SIMD register for optimized operations (16-byte aligned) */
    //     alignas(16) __m128 data;
    // #endif

    //     /** @brief Array access for direct component manipulation (16-byte aligned) */
    f32 elements[4];

    /** @brief Named component access */
    union {
        struct {
            union {
                /** @brief First component (x position, red channel, s texture coord) */
                f32 x, r, s;
            };
            union {
                /** @brief Second component (y position, green channel, t texture coord) */
                f32 y, g, t;
            };
            union {
                /** @brief Third component (z position, blue channel, p texture coord) */
                f32 z, b, p;
            };
            union {
                /** @brief Fourth component (w position, alpha channel, q texture coord) */
                f32 w, a, q;
            };
        };
    };
} vec4;

/**
 * @brief Quaternion type alias using vec4 storage.
 *
 * Quaternions are stored as:
 * - x,y,z: Vector components.
 * - w: Scalar component.
 * Provides identical memory layout to vec4 for interoperability.
 *
 * Why needed:
 * - Memory layout matches vec4 for efficient conversions
 * - Avoids separate storage type while maintaining quaternion semantics
 * - Enables using vector operations on quaternion components
 */
typedef vec4 quat;

/**
 * @brief A 4x4 matrix union for linear transformations.
 *
 * Stored in column-major order:
 * ```sh
 * [
 *   0, 4,  8, 12,
 *   1, 5,  9, 13,
 *   2, 6, 10, 14,
 *   3, 7, 11, 15
 * ]
 *```
 *
 * Why needed:
 * - Standard representation for 3D transformations (translation, rotation, scale)
 * - Column-major matches OpenGL/Vulkan conventions
 * - Raw array access for direct GPU buffer uploads
 * - Basis for camera/view/projection matrices
 */
typedef union mat4_u {
    /** @brief Flat array of 16 elements in column-major order */
    f32 data[16];
} mat4;

/**
 * @brief A 3D vertex structure.
 *
 * Represents a vertex in 3D space with position data.
 * Can be extended to include normals, texture coordinates, etc.
 *
 * Why needed:
 * - Standard representation for 3D vertices in graphics applications
 * - Simplifies passing vertex data to rendering pipelines
 * - Can be easily extended for additional attributes
 */
typedef struct vertex_3d {
    vec3 position;  ///< Vertex position in 3D space
} vertex_3d;