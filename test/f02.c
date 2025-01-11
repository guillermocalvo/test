
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

    TRY {

        THROW(NullPointerException, "I'm going to be caught.");

    } CATCH(RuntimeException) {

        caught = true;

    }

    TEST_ASSERT(caught);
}
