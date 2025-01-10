
# include "testing.h"


/**
 * Catching a generic exception
 *
 * This test starts a `try` block, throws `NullPointerException` and catches
 * it with a `catch(RuntimeException)` block.
 *
 */
TEST_CASE{

    volatile bool caught = false;

    E4C_TRY {

        E4C_THROW(NullPointerException, "I'm going to be caught.");

    } E4C_CATCH(RuntimeException) {

        caught = true;

    }

    TEST_ASSERT(caught);
}
