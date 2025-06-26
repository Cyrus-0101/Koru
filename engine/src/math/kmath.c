#include "kmath.h"
#include "platform/platform.h"

#include <math.h>
#include <stdlib.h>

/**
 * @file kmath.c
 * @brief Math utilities for the Koru Engine.
 *
 * This module wraps basic math functions from <math.h> to avoid importing it everywhere.
 * It also provides random number generation functionality with optional range support.
 *
 * All functions are safe to call before full engine initialization,
 * except those that require seeded randomness.
 */

/**
 * Note that these are here in order to prevent having to import the
 * entire <math.h> everywhere.
 */

// Static state
static b8 rand_seeded = False;

/**
 * @brief Wraps sinf(x) from <math.h>.
 *
 * Returns the sine of x radians.
 *
 * @param x The angle in radians.
 * @return f32 The sine of x.
 */
f32 ksin(f32 x) {
    return sinf(x);
}

/**
 * @brief Wraps cosf(x) from <math.h>.
 *
 * Returns the cosine of x radians.
 *
 * @param x The angle in radians.
 * @return f32 The cosine of x.
 */
f32 kcos(f32 x) {
    return cosf(x);
}

/**
 * @brief Wraps tanf(x) from <math.h>.
 *
 * Returns the tangent of x radians.
 *
 * @param x The angle in radians.
 * @return f32 The tangent of x.
 */
f32 ktan(f32 x) {
    return tanf(x);
}

/**
 * @brief Wraps acosf(x) from <math.h>.
 *
 * Returns the arc cosine of x, in radians.
 *
 * @param x A value between -1 and 1.
 * @return f32 The arc cosine of x.
 */
f32 kacos(f32 x) {
    return acosf(x);
}

/**
 * @brief Wraps sqrtf(x) from <math.h>.
 *
 * Returns the square root of x.
 *
 * @param x A non-negative value.
 * @return f32 The square root of x.
 */
f32 ksqrt(f32 x) {
    return sqrtf(x);
}/**
 * Note that these are here in order to prevent having to import the
 * entire <math.h> everywhere.
 */

/**
 * @brief Wraps fabsf(x) from <math.h>.
 *
 * Returns the absolute value of x.
 *
 * @param x The input value.
 * @return f32 The absolute value of x.
 */
f32 kabs(f32 x) {
    return fabsf(x);
}

/**
 * @brief Generates a random integer between 0 and RAND_MAX.
 *
 * Automatically seeds using platform time if not already seeded.
 *
 * @return i32 A pseudo-random integer.
 */
i32 krandom() {
    if (!rand_seeded) {
        srand((u32)platform_get_absolute_time());
        rand_seeded = True;
    }
    return rand();
}

/**
 * @brief Generates a random integer within the given range [min, max].
 *
 * If min > max, values are swapped internally.
 *
 * @param min The lower bound of the range.
 * @param max The upper bound of the range.
 * @return i32 A pseudo-random integer in range [min, max]
 */
i32 krandom_in_range(i32 min, i32 max) {
    if (!rand_seeded) {
        srand((u32)platform_get_absolute_time());
        rand_seeded = True;
    }

    // Swap if min > max
    // if (min > max) {
    //     i32 temp = min;
    //     min = max;
    //     max = temp;
    // }

    return (rand() % (max - min + 1)) + min;
}

/**
 * @brief Returns a floating-point random value in range [0.0f, 1.0f]
 *
 * Seeded automatically on first call.
 *
 * @return f32 A random float between 0.0f and 1.0f
 */
f32 fkrandom() {
    return (float)krandom() / (f32)RAND_MAX;
}

/**
 * @brief Returns a floating-point random value in range [min, max]
 *
 * Seeded automatically on first call.
 *
 * @param min The lower bound of the desired range.
 * @param max The upper bound of the desired range.
 * @return f32 A random float between min and max.
 */
f32 fkrandom_in_range(f32 min, f32 max) {
    return min + ((float)krandom() / ((f32)RAND_MAX / (max - min)));
}