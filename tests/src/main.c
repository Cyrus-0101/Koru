#include "test_manager.h"

#include "memory/linear_allocator_tests.h"

#include <core/logger.h>

/**
 * @file main.c
 * @brief Entry point for the testbed application.
 *
 * Initializes the test manager and runs all registered tests.
 * Intended for unit testing core engine systems before full game initialization.
 */

/**
 * @brief Application entry point for running tests.
 *
 * Initializes the test manager, registers tests, and executes them.
 *
 * @return int Standard exit code (always returns 0 currently).
 */
int main() {
    // Initalize the test manager.
    test_manager_init();

    // Add test registrations here.
    linear_allocator_register_tests();


    KDEBUG("Starting tests...");

    // Execute tests
    test_manager_run_tests();

    return 0;
}