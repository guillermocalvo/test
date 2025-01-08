
# include "testing.h"


/**
 * Catching a specific exception
 *
 * This test starts a `try` block, throws `NullPointerException` and catches
 * it with a `catch(IllegalArgumentException)` block.
 *
 */
TEST_CASE{

    volatile bool caught = false;

    e4c_context_begin();

    E4C_TRY{

        E4C_THROW(NullPointerException, "I'm going to be caught.");

    }E4C_CATCH(NullPointerException){

        caught = true;
    }

    e4c_context_end();

    TEST_ASSERT(caught);
}
