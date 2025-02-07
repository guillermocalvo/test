
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
static const struct e4c_exception_type NullPointerException = {&RuntimeException, "Null pointer."};

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

    } CATCH (RuntimeException) {

        caught = true;

    }

    TEST_ASSERT(caught);
}
