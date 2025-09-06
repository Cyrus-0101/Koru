#include "hashtable_tests.h"
#include "../test_manager.h"
#include "../expect.h"

#include <defines.h>
#include <containers/hashtable.h>

/**
 * @file hashtable_tests.c
 * @brief Unit tests for the hashtable subsystem.
 *
 * These tests validate core functionality of the `hashtable` including:
 * - Creation and destruction
 * - Setting and getting values
 *
 * Uses the custom test manager and assertion macros from `test_manager.h`.
 */

/**
 * @brief Tests that a hashtable can be created and destroyed properly.
 *
 * Verifies:
 * - Memory pointer is valid after creation
 * - Size fields are initialized correctly
 * - After destruction, all state is reset
 */
u8 hashtable_should_create_and_destroy() {
    // Arrange
    hashtable table;
    u64 element_size = sizeof(u64);
    u32 element_count = 3;
    u64 memory[3];

    // Act
    hashtable_create(element_size, element_count, memory, False, &table);

    // Assert
    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(u64), table.element_size);
    expect_should_be(3, table.element_count);
    expect_should_be(False, table.is_pointer_type);

    // Cleanup
    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);
    expect_should_be(0, table.is_pointer_type);

    return True;
}

u8 hashtable_should_set_and_get_successfully() {
    // Arrange
    hashtable table;
    u64 element_size = sizeof(u64);
    u64 element_count = 3;
    u64 memory[3];

    // Act
    hashtable_create(element_size, element_count, memory, False, &table);

    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(u64), table.element_size);
    expect_should_be(3, table.element_count);
    expect_should_be(False, table.is_pointer_type);

    // Assert
    u64 testval1 = 42;
    hashtable_set(&table, "test1", &testval1);
    u64 get_testval_1 = 0;
    hashtable_get(&table, "test1", &get_testval_1);
    expect_should_be(testval1, get_testval_1);

    // Cleanup
    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);
    expect_should_be(0, table.is_pointer_type);

    return True;
}

/**
 *
 * @struct ht_test_struct
 * @brief A test structure for hashtable pointer tests.
 */
typedef struct ht_test_struct {
    /** A boolean value. */
    b8 b_value;
    /** A floating-point value. */
    f32 f_value;
    /** An unsigned integer value. */
    u64 u_value;
} ht_test_struct;

/**
 * @brief Tests that a hashtable can set and get pointer types successfully.
 *
 * Verifies:
 * - Pointer values are stored and retrieved correctly
 * - Retrieved values match the original values
 */
u8 hashtable_should_set_and_get_ptr_successfully() {
    hashtable table;
    u64 element_size = sizeof(ht_test_struct*);
    u64 element_count = 3;
    ht_test_struct* memory[3];

    hashtable_create(element_size, element_count, memory, True, &table);

    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(ht_test_struct*), table.element_size);
    expect_should_be(3, table.element_count);

    ht_test_struct t;
    ht_test_struct* testval1 = &t;
    testval1->b_value = True;
    testval1->u_value = 63;
    testval1->f_value = 3.1415f;
    hashtable_set_ptr(&table, "test1", (void**)&testval1);

    ht_test_struct* get_testval_1 = 0;
    hashtable_get_ptr(&table, "test1", (void**)&get_testval_1);

    expect_should_be(testval1->b_value, get_testval_1->b_value);
    expect_should_be(testval1->u_value, get_testval_1->u_value);

    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);

    return True;
}

/**
 * @brief Tests setting and getting a non-existent key in the hashtable.
 *
 * Verifies:
 * - Getting a non-existent key returns a default value (0 for integers)
 * - No errors occur during the process
 */
u8 hashtable_should_set_and_get_nonexistant() {
    hashtable table;
    u64 element_size = sizeof(u64);
    u64 element_count = 3;
    u64 memory[3];

    hashtable_create(element_size, element_count, memory, False, &table);

    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(u64), table.element_size);
    expect_should_be(3, table.element_count);

    u64 testval1 = 23;
    hashtable_set(&table, "test1", &testval1);
    u64 get_testval_1 = 0;
    hashtable_get(&table, "test2", &get_testval_1);
    expect_should_be(0, get_testval_1);

    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);

    return True;
}

/**
 * @brief Tests setting and getting a non-existent pointer key in the hashtable.
 *
 * Verifies:
 * - Getting a non-existent pointer key returns null
 * - No errors occur during the process
 */
u8 hashtable_should_set_and_get_ptr_nonexistant() {
    hashtable table;
    u64 element_size = sizeof(ht_test_struct*);
    u64 element_count = 3;
    ht_test_struct* memory[3];

    hashtable_create(element_size, element_count, memory, True, &table);

    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(ht_test_struct*), table.element_size);
    expect_should_be(3, table.element_count);

    ht_test_struct t;
    ht_test_struct* testval1 = &t;
    testval1->b_value = True;
    testval1->u_value = 63;
    testval1->f_value = 3.1415f;
    b8 result = hashtable_set_ptr(&table, "test1", (void**)&testval1);
    expect_to_be_true(result);

    ht_test_struct* get_testval_1 = 0;
    result = hashtable_get_ptr(&table, "test2", (void**)&get_testval_1);
    expect_to_be_false(result);
    expect_should_be(0, get_testval_1);

    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);

    return True;
}

/**
 * @brief Tests setting and unsetting a pointer value in the hashtable.
 *
 * Verifies:
 * - A pointer can be set and retrieved correctly
 * - The pointer can be unset (set to null)
 * - After unsetting, retrieving the pointer returns null
 */
u8 hashtable_should_set_and_unset_ptr() {
    hashtable table;
    u64 element_size = sizeof(ht_test_struct*);
    u64 element_count = 3;
    ht_test_struct* memory[3];

    hashtable_create(element_size, element_count, memory, True, &table);

    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(ht_test_struct*), table.element_size);
    expect_should_be(3, table.element_count);

    ht_test_struct t;
    ht_test_struct* testval1 = &t;
    testval1->b_value = True;
    testval1->u_value = 63;
    testval1->f_value = 3.1415f;
    // Set it
    b8 result = hashtable_set_ptr(&table, "test1", (void**)&testval1);
    expect_to_be_true(result);

    // Check that it exists and is correct.
    ht_test_struct* get_testval_1 = 0;
    hashtable_get_ptr(&table, "test1", (void**)&get_testval_1);
    expect_should_be(testval1->b_value, get_testval_1->b_value);
    expect_should_be(testval1->u_value, get_testval_1->u_value);

    // Unset it
    result = hashtable_set_ptr(&table, "test1", 0);
    expect_to_be_true(result);

    // Should no longer be found.
    ht_test_struct* get_testval_2 = 0;
    result = hashtable_get_ptr(&table, "test1", (void**)&get_testval_2);
    expect_to_be_false(result);
    expect_should_be(0, get_testval_2);

    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);

    return True;
}

/**
 * @brief Tests that calling non-pointer functions on a pointer-type hashtable fails gracefully.
 *
 * Verifies:
 * - Attempting to set or get using non-pointer functions on a pointer-type hashtable returns false
 * - Appropriate error messages are logged
 */
u8 hashtable_try_call_non_ptr_on_ptr_table() {
    hashtable table;
    u64 element_size = sizeof(ht_test_struct*);
    u64 element_count = 3;
    ht_test_struct* memory[3];

    hashtable_create(element_size, element_count, memory, True, &table);

    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(ht_test_struct*), table.element_size);
    expect_should_be(3, table.element_count);

    KDEBUG("The following 2 error messages are intentional.");

    ht_test_struct t;
    t.b_value = True;
    t.u_value = 63;
    t.f_value = 3.1415f;
    // Try setting the record
    b8 result = hashtable_set(&table, "test1", &t);
    expect_to_be_false(result);

    // Try getting the record.
    ht_test_struct* get_testval_1 = 0;
    result = hashtable_get(&table, "test1", (void**)&get_testval_1);
    expect_to_be_false(result);

    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);

    return True;
}

/**
 * @brief Tests that calling pointer functions on a non-pointer-type hashtable fails gracefully.
 *
 * Verifies:
 * - Attempting to set or get using pointer functions on a non-pointer-type hashtable returns false
 * - Appropriate error messages are logged
 */
u8 hashtable_try_call_ptr_on_non_ptr_table() {
    hashtable table;
    u64 element_size = sizeof(ht_test_struct);
    u64 element_count = 3;
    ht_test_struct memory[3];

    hashtable_create(element_size, element_count, memory, False, &table);

    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(ht_test_struct), table.element_size);
    expect_should_be(3, table.element_count);

    KDEBUG("The following 2 error messages are intentional.");

    ht_test_struct t;
    ht_test_struct* testval1 = &t;
    testval1->b_value = True;
    testval1->u_value = 63;
    testval1->f_value = 3.1415f;
    // Attempt to call pointer functions.
    b8 result = hashtable_set_ptr(&table, "test1", (void**)&testval1);
    expect_to_be_false(result);

    // Try to call pointer function.
    ht_test_struct* get_testval_1 = 0;
    result = hashtable_get_ptr(&table, "test1", (void**)&get_testval_1);
    expect_to_be_false(result);

    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);

    return True;
}

/**
 * @brief Tests getting a pointer from the hashtable, updating the pointed-to values, and getting it again.
 *
 * Verifies:
 * - A pointer can be set and retrieved correctly
 * - The pointed-to values can be updated
 * - Retrieving the pointer again reflects the updated values
 */
u8 hashtable_should_set_get_and_update_ptr_successfully() {
    hashtable table;
    u64 element_size = sizeof(ht_test_struct*);
    u64 element_count = 3;
    ht_test_struct* memory[3];

    hashtable_create(element_size, element_count, memory, True, &table);

    expect_should_not_be(0, table.memory);
    expect_should_be(sizeof(ht_test_struct*), table.element_size);
    expect_should_be(3, table.element_count);

    ht_test_struct t;
    ht_test_struct* testval1 = &t;
    testval1->b_value = True;
    testval1->u_value = 63;
    testval1->f_value = 3.1415f;
    hashtable_set_ptr(&table, "test1", (void**)&testval1);

    ht_test_struct* get_testval_1 = 0;
    hashtable_get_ptr(&table, "test1", (void**)&get_testval_1);
    expect_should_be(testval1->b_value, get_testval_1->b_value);
    expect_should_be(testval1->u_value, get_testval_1->u_value);

    // Update pointed-to values
    get_testval_1->b_value = False;
    get_testval_1->u_value = 99;
    get_testval_1->f_value = 6.69f;

    // Get the pointer again and confirm correct values
    ht_test_struct* get_testval_2 = 0;
    hashtable_get_ptr(&table, "test1", (void**)&get_testval_2);
    expect_to_be_false(get_testval_2->b_value);
    expect_should_be(99, get_testval_2->u_value);
    expect_float_to_be(6.69f, get_testval_2->f_value);

    hashtable_destroy(&table);

    expect_should_be(0, table.memory);
    expect_should_be(0, table.element_size);
    expect_should_be(0, table.element_count);

    return True;
}

void hashtable_allocate_tests() {
    test_manager_register_test(hashtable_should_create_and_destroy, "Hashtable should create and destroy properly");
    test_manager_register_test(hashtable_should_set_and_get_successfully, "Hashtable should set and get successfully");
    test_manager_register_test(hashtable_should_set_and_get_ptr_successfully, "Hashtable should set and get pointer");
    test_manager_register_test(hashtable_should_set_and_get_nonexistant, "Hashtable should set and get non-existent entry as nothing.");
    test_manager_register_test(hashtable_should_set_and_get_ptr_nonexistant, "Hashtable should set and get non-existent pointer entry as nothing.");
    test_manager_register_test(hashtable_should_set_and_unset_ptr, "Hashtable should set and unset pointer entry as nothing.");
    test_manager_register_test(hashtable_try_call_non_ptr_on_ptr_table, "Hashtable try calling non-pointer functions on pointer type table.");
    test_manager_register_test(hashtable_try_call_ptr_on_non_ptr_table, "Hashtable try calling pointer functions on non-pointer type table.");
    test_manager_register_test(hashtable_should_set_get_and_update_ptr_successfully, "Hashtable Should get pointer, update, and get again successfully.");
}
