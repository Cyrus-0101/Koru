#include "linear_allocator_tests.h"
#include "../test_manager.h"
#include "../expect.h"

#include <defines.h>
#include <memory/linear_allocator.h>

/**
 * @file linear_allocator_tests.c
 * @brief Unit tests for the linear allocator subsystem.
 *
 * These tests validate core functionality of the `linear_allocator` including:
 * - Creation and destruction
 * - Single allocation
 * - Multiple allocations up to capacity
 * - Handling out-of-memory conditions
 * - Resetting all allocations
 *
 * Uses the custom test manager and assertion macros from `test_manager.h`.
 */

/**
 * @brief Tests that a linear allocator can be created and destroyed properly.
 *
 * Verifies:
 * - Memory pointer is valid after creation
 * - Size fields are initialized correctly
 * - After destruction, all state is reset
 */
u8 linear_allocator_should_create_and_destroy() {
    linear_allocator alloc;
    linear_allocator_create(sizeof(u64), 0, &alloc);

    expect_should_not_be(0, alloc.memory);
    expect_should_be(sizeof(u64), alloc.total_size);
    expect_should_be(0, alloc.allocated);

    linear_allocator_destroy(&alloc);

    expect_should_be(0, alloc.memory);
    expect_should_be(0, alloc.total_size);
    expect_should_be(0, alloc.allocated);

    return True;
}

/**
 * @brief Tests allocating the entire size of the allocator in one call.
 *
 * Ensures:
 * - Allocation returns non-null
 * - The allocated size matches total size
 */
u8 linear_allocator_single_allocation_all_space() {
    linear_allocator alloc;
    linear_allocator_create(sizeof(u64), 0, &alloc);

    // Single allocation.
    void* block = linear_allocator_allocate(&alloc, sizeof(u64));

    // Validate it
    expect_should_not_be(0, block);
    expect_should_be(sizeof(u64), alloc.allocated);

    linear_allocator_destroy(&alloc);

    return True;
}

/**
 * @brief Tests multiple allocations until the allocator is completely filled.
 *
 * Allocates max_allocs times, each of size u64.
 * Validates:
 * - Each allocation is successful
 * - Allocated size increases as expected
 */
u8 linear_allocator_multi_allocation_all_space() {
    u64 max_allocs = 1024;
    linear_allocator alloc;
    linear_allocator_create(sizeof(u64) * max_allocs, 0, &alloc);

    // Multiple allocations - full.
    void* block;
    for (u64 i = 0; i < max_allocs; ++i) {
        block = linear_allocator_allocate(&alloc, sizeof(u64));
        // Validate it
        expect_should_not_be(0, block);
        expect_should_be(sizeof(u64) * (i + 1), alloc.allocated);
    }

    linear_allocator_destroy(&alloc);

    return True;
}

/**
 * @brief Tests attempting to allocate beyond the capacity of the allocator.
 *
 * Allocates up to full capacity, then tries one more allocation.
 * Expected behavior:
 * - All initial allocations succeed
 * - Final allocation fails (returns 0)
 * - Total allocated size remains unchanged
 */
u8 linear_allocator_multi_allocation_over_allocate() {
    u64 max_allocs = 3;
    linear_allocator alloc;
    linear_allocator_create(sizeof(u64) * max_allocs, 0, &alloc);

    // Multiple allocations - full.
    void* block;
    for (u64 i = 0; i < max_allocs; ++i) {
        block = linear_allocator_allocate(&alloc, sizeof(u64));
        // Validate it
        expect_should_not_be(0, block);
        expect_should_be(sizeof(u64) * (i + 1), alloc.allocated);
    }

    KDEBUG("Note: The following error is intentionally caused by this test.");

    // Ask for one more allocation. Should error and return 0.
    block = linear_allocator_allocate(&alloc, sizeof(u64));
    // Validate it - allocated should be unchanged.
    expect_should_be(0, block);
    expect_should_be(sizeof(u64) * (max_allocs), alloc.allocated);

    linear_allocator_destroy(&alloc);

    return True;
}

/**
 * @brief Tests allocating fully, then calling free_all resets the pointer.
 *
 * Validates:
 * - Full allocation works
 * - `free_all()` resets the allocated counter to 0
 */
u8 linear_allocator_multi_allocation_all_space_then_free() {
    u64 max_allocs = 1024;
    linear_allocator alloc;
    linear_allocator_create(sizeof(u64) * max_allocs, 0, &alloc);

    // Multiple allocations - full.
    void* block;
    for (u64 i = 0; i < max_allocs; ++i) {
        block = linear_allocator_allocate(&alloc, sizeof(u64));
        // Validate it
        expect_should_not_be(0, block);
        expect_should_be(sizeof(u64) * (i + 1), alloc.allocated);
    }

    // Validate that pointer is reset.
    linear_allocator_free_all(&alloc);
    expect_should_be(0, alloc.allocated);

    linear_allocator_destroy(&alloc);

    return True;
}

void linear_allocator_register_tests() {
    test_manager_register_test(linear_allocator_should_create_and_destroy, "Linear allocator should create and destroy");
    test_manager_register_test(linear_allocator_single_allocation_all_space, "Linear allocator single alloc for all space");
    test_manager_register_test(linear_allocator_multi_allocation_all_space, "Linear allocator multi alloc for all space");
    test_manager_register_test(linear_allocator_multi_allocation_over_allocate, "Linear allocator try over allocate");
    test_manager_register_test(linear_allocator_multi_allocation_all_space_then_free, "Linear allocator allocated should be 0 after free_all");
}