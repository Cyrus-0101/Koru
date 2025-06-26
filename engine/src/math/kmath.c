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

f32 ksin(f32 x) {
    return sinf(x);
}

f32 kcos(f32 x) {
    return cosf(x);
}

f32 ktan(f32 x) {
    return tanf(x);
}

f32 kacos(f32 x) {
    return acosf(x);
}

f32 ksqrt(f32 x) {
    return sqrtf(x);
}

/**
 * Note that these are here in order to prevent having to import the
 * entire <math.h> everywhere.
 */

f32 kabs(f32 x) {
    return fabsf(x);
}

i32 krandom() {
    if (!rand_seeded) {
        srand((u32)platform_get_absolute_time());
        rand_seeded = True;
    }
    return rand();
}

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

f32 fkrandom() {
    return (float)krandom() / (f32)RAND_MAX;
}

f32 fkrandom_in_range(f32 min, f32 max) {
    return min + ((float)krandom() / ((f32)RAND_MAX / (max - min)));
}