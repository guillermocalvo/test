
# include "testing.h"


void another_function(volatile bool * flag);


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

    e4c_context_begin();

    E4C_TRY{

        another_function(&cleanup);

    }E4C_CATCH(RuntimeException){

        TEST_ASSERT(  e4c_is_instance_of(e4c_get_exception(), &NullPointerException) );
    }

    TEST_ASSERT(cleanup);

    e4c_context_end();
}


void another_function(volatile bool * flag){


    E4C_TRY{

        E4C_THROW(NullPointerException, "Get me out of here.");

    }E4C_CATCH(RuntimeException){

        E4C_THROW(NullPointerException, "Told you to get me out of here.");

    }E4C_FINALLY{

        *flag = true;
    }

    *flag = false;
}
