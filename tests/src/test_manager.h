#pragma once

#include <defines.h>

/**
 * @file test_manager.h
 * @brief Test management interface for the Koru Engine's internal testing framework.
 *
 * This module provides:
 * - A way to register tests
 * - A manager to run them in sequence
 * - Basic reporting (passed/failed/skipped)
 *
 * Designed specifically for testing low-level systems like memory allocators,
 * containers, and math utilities before full engine integration.
 */

/**
 * @def BYPASS
 * @brief Return value indicating a test was skipped intentionally.
 *
 * Used by test functions that are temporarily bypassed.
 */
#define BYPASS 2

/**
 * @typedef PFN_test
 * @brief Function pointer type for test functions.
 *
 * Test functions must return u8:
 * - 0 = Success
 * - 1 = Failure
 * - 2 = Bypassed
 */
typedef u8 (*PFN_test)();

/**
 * @brief Initializes the test manager.
 *
 * Must be called before registering or running any tests.
 */
void test_manager_init();

/**
 * @brief Registers a test function with an associated description.
 *
 * @param func The test function to register.
 * @param desc Human-readable description of what this test does.
 */
void test_manager_register_test(PFN_test func, char* desc);

/**
 * @brief Runs all registered tests and reports results.
 *
 * Includes time tracking per test and total execution time.
 * Uses the logger system to print results to the console.
 */
void test_manager_run_tests();