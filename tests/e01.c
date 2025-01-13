
# include "testing.h"


void another_function(volatile bool * flag);

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
static const struct e4c_exception_type NullPointerException = {&RuntimeException, "Null pointer."};

/**
 * Uncaught exception with a `finally` block
 *
 * This test checks the execution of a `finally` block.
 *
 * The expected behavior is:
 *
 *   - The test starts a `try` block with a `catch` block.
 *   - A function is called from the `try` block.
 *     - The function starts a `try` block with a `finally` block.
 *     - An exception is thrown from the `try` block.
 *     - There is no `catch` block to handle it.
 *     - The `finally` block is executed.
 *   - The outter `catch` block catches the exception.
 *
 */
TEST_CASE{

    volatile bool cleanup = false;

    TRY {

        another_function(&cleanup);

    } CATCH(RuntimeException) {

        TEST_ASSERT_EQUALS(e4c_get_exception()->type, &NullPointerException);
    }

    TEST_ASSERT(cleanup);
}

void another_function(volatile bool * flag){

    TRY {

        THROW(NullPointerException, "Get me out of here.");

    } FINALLY {

        *flag = true;
    }

    *flag = false;
}
