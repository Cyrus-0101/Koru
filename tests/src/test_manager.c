#include "test_manager.h"

#include <core/clock.h>
#include <containers/darray.h>
#include <core/kstring.h>
#include <core/logger.h>

/**
 * @file test_manager.c
 * @brief Implementation of the test manager for the Koru Engine.
 *
 * This module implements basic test registration and execution functionality.
 * It uses dynamic arrays (`darray`) to store test entries and logs via KINFO/KERROR.
 */


/**
 * @struct test_entry
 * @brief Represents a single test case entry.
 *
 * Stores:
 * - The test function pointer
 * - A human-readable description of the test
 */
typedef struct test_entry {
    /**
     * @brief Function pointer to the test function.
     */
    PFN_test func;

    /**
     * @brief Description of the test (for logging).
     */
    char* desc;
} test_entry;

/**
 * @brief Static array of registered test cases.
 *
 * Managed using darray for dynamic resizing.
 */
static test_entry* tests;

/**
 * @brief Initializes the test manager.
 *
 * Clears and creates a new dynamic array to hold test entries.
 */
void test_manager_init() {
    tests = darray_create(test_entry);
}

/**
 * @brief Registers a test function with a description.
 *
 * @param func The function pointer to the test case.
 * @param desc Description of the test (e.g., "Test: Memory allocation edge cases")
 */
void test_manager_register_test(u8 (*PFN_test)(), char* desc) {
    test_entry e;
    e.func = PFN_test;
    e.desc = desc;
    darray_push(tests, e);
}

/**
 * @brief Runs all registered tests and prints status updates.
 *
 * Loops through all registered test functions and:
 * - Starts timing the individual test
 * - Calls the test function
 * - Logs result (pass/fail/skip)
 * - Updates global count
 *
 * Reports final results after all tests complete.
 */
void test_manager_run_tests() {
    u32 passed = 0;
    u32 failed = 0;
    u32 skipped = 0;

    u32 count = darray_length(tests);

    clock total_time;
    clock_start(&total_time);

    for (u32 i = 0; i < count; ++i) {
        clock test_time;
        clock_start(&test_time);
        u8 result = tests[i].func();
        clock_update(&test_time);

        if (result == True) {
            ++passed;
        } else if (result == BYPASS) {
            KWARN("[SKIPPED]: %s", tests[i].desc);
            ++skipped;
        } else {
            KERROR("[FAILED]: %s", tests[i].desc);
            ++failed;
        }
        
        char status[20];
        string_format(status, failed ? "*** %d FAILED ***" : "SUCCESS", failed);
        clock_update(&total_time);
        KINFO("Executed %d of %d (skipped %d) %s (%.6f sec / %.6f sec total", i + 1, count, skipped, status, test_time.elapsed, total_time.elapsed);
    }

    clock_stop(&total_time);

    KINFO("Results: %d passed, %d failed, %d skipped.", passed, failed, skipped);
}