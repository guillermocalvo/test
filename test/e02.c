
# include "testing.h"


void another_function(volatile bool * flag1, volatile bool * flag2);
void yet_another_function(volatile bool * flag2);


/**
 * Uncaught exception with a pair of `finally` blocks
 *
 * This test checks the execution of two consecutive `finally` blocks.
 *
 * The expected behavior is:
 *
 *   - The test starts a `try` block with a `catch` block.
 *   - A function is called from the `try` block.
 *     - The function starts a `try` block with a `finally` #1 block.
 *     - Another function is called from the `try` block.
 *       - The function starts a `try` block with a `finally` #2 block.
 *       - The function throws an exception from its `try` block.
 *       - There is no `catch` block to handle it.
 *       - The `finally` #2 block (of the function) is executed.
 *     - The `finally` #1 block (of the test) is executed.
 *   - The outter `catch` block catches the exception.
 *
 */
TEST_CASE{

    volatile bool cleanup1 = false;
    volatile bool cleanup2 = false;

    TRY {

        another_function(&cleanup1, &cleanup2);

    } CATCH(RuntimeException) {

        TEST_ASSERT(  e4c_is_instance_of(e4c_get_exception(), &NullPointerException) );
    }

    TEST_ASSERT(cleanup1);
    TEST_ASSERT(cleanup2);
}

void another_function(volatile bool * flag1, volatile bool * flag2){

    TRY {

        yet_another_function(flag2);

    } FINALLY {

        *flag1 = true;
    }

    *flag1 = false;
}

void yet_another_function(volatile bool * flag2){

    TRY {

        THROW(NullPointerException, "Get me out of here.");

    } FINALLY {

        *flag2 = true;
    }

    *flag2 = false;
}
