#include <core/logger.h>
#include <math/kmath.h>

/**
 * @file expect.h
 * @brief Assertion macros used across unit tests.
 *
 * These macros help write cleaner test logic by abstracting common checks:
 * - Equality
 * - Inequality
 * - Boolean values
 * - Floating-point comparisons
 *
 * If a condition fails, they log an error and return False from the current test.
 */


/**
 * @def expect_should_be(expected, actual)
 * @brief Asserts that two integer values are equal.
 *
 * If not equal, logs an error and returns False.
 *
 * @param expected Expected value
 * @param actual Actual value returned by the test
 */
#define expect_should_be(expected, actual)                                                              \
    if (actual != expected) {                                                                           \
        KERROR("--> Expected %lld, but got: %lld. File: %s:%d.", expected, actual, __FILE__, __LINE__); \
        return False;                                                                                   \
    }

/**
 * @def expect_should_not_be(expected, actual)
 * @brief Asserts that two integer values are NOT equal.
 *
 * If they are equal, logs an error and returns False.
 *
 * @param expected Value that should not match actual
 * @param actual Value being tested
 */
#define expect_should_not_be(expected, actual)                                                                   \
    if (actual == expected) {                                                                                    \
        KERROR("--> Expected %d != %d, but they are equal. File: %s:%d.", expected, actual, __FILE__, __LINE__); \
        return False;                                                                                            \
    }

/**
 * @def expect_float_to_be(expected, actual)
 * @brief Compares floating-point values within a small tolerance.
 *
 * Uses `kabs(expected - actual) > 0.001f` as epsilon check.
 *
 * @param expected Expected float value
 * @param actual Float value to compare against
 */
#define expect_float_to_be(expected, actual)                                                        \
    if (kabs(expected - actual) > 0.001f) {                                                         \
        KERROR("--> Expected %f, but got: %f. File: %s:%d.", expected, actual, __FILE__, __LINE__); \
        return False;                                                                               \
    }

/**
 * @def expect_to_be_true(actual)
 * @brief Checks that a boolean value is TRUE.
 *
 * If FALSE, logs an error and returns False from the test function.
 *
 * @param actual Value to test.
 */
#define expect_to_be_true(actual)                                                      \
    if (actual != True) {                                                              \
        KERROR("--> Expected True, but got: False. File: %s:%d.", __FILE__, __LINE__); \
        return False;                                                                  \
    }

/**
 * @def expect_to_be_false(actual)
 * @brief Checks that a boolean value is FALSE.
 *
 * If TRUE, logs an error and returns False from the test function.
 *
 * @param actual Value to test.
 */
#define expect_to_be_false(actual)                                                     \
    if (actual != False) {                                                             \
        KERROR("--> Expected False, but got: true. File: %s:%d.", __FILE__, __LINE__); \
        return False;                                                                  \
    }