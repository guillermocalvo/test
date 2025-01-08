
# include "testing.h"


/**
 * Catching a generic exception
 *
 * This test starts a `try` block, throws `IllegalArgumentException` and catches
 * it with a `catch(RuntimeException)` block.
 *
 */
TEST_CASE{

    volatile bool caught = false;

    e4c_using_context {

        E4C_TRY{

            E4C_THROW(IllegalArgumentException, "I'm going to be caught.");

        }E4C_CATCH(RuntimeException){

            caught = true;

        }
    }

    TEST_ASSERT(caught);
}
