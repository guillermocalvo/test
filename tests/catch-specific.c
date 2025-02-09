
# include "testing.h"

static const struct e4c_exception_type SPECIFIC = {NULL, "Specific exception"};

/**
 * Catching a specific exception
 *
 * This test starts a `try` block, throws `NullPointerException` and catches
 * it with a `catch(IllegalArgumentException)` block.
 *
 */
TEST_CASE{

    volatile bool caught = false;

    TRY {
        THROW(SPECIFIC, NULL);
    } CATCH (SPECIFIC) {
        caught = true;
    }

    TEST_ASSERT(caught);
}
