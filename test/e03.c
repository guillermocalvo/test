
# include "testing.h"


void another_function(volatile bool * flag);

static const struct e4c_exception_type NullPointerException = {&RuntimeException, "Null pointer."};

/**
 * Uncaught exception, thrown from a `catch` block, with a `finally` block
 *
 * This test checks the execution of a `finally` block when an exception is
 * thrown from a preceding `catch` block.
 *
 * The expected behavior is:
 *
 *   - The test starts a `try` block with a `catch` block.
 *   - A function is called from the `try` block.
 *     - The function starts a `try` block with a `catch` and a `finally` block.
 *     - An exception is thrown from the `try` block.
 *     - The `catch` block handles it.
 *     - An exception is thrown from the `catch` block.
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

    } CATCH(RuntimeException) {

        THROW(NullPointerException, "Told you to get me out of here.");

    } FINALLY {

        *flag = true;
    }

    *flag = false;
}
