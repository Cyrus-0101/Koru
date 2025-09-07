#pragma once

/**
 * @file hashtable_tests.h
 * @brief Unit tests for the hashtable implementation.
 *
 * Contains function declarations for various hashtable tests.
 * All tests are registered via `hashtabel_allocate_tests()`.
 */

/**
 * @brief Registers all hashtable tests with the test manager.
 *
 * Should be called before `test_manager_run_tests()` in main().
 */
void hashtable_allocate_tests();