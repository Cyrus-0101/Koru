#pragma once

/**
 * @file linear_allocator_tests.h
 * @brief Unit tests for the linear allocator implementation.
 *
 * Contains function declarations for various linear allocator tests.
 * All tests are registered via `linear_allocator_register_tests()`.
 */

/**
 * @brief Registers all linear allocator tests with the test manager.
 *
 * Should be called before `test_manager_run_tests()` in main().
 */
void linear_allocator_register_tests();