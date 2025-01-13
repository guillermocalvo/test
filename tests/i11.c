
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Getting status (failed)
 *
 * This test retrieves the completeness of a code block. There was an uncaught
 * exceptions during the `try` block, so the status is `e4c_failed`. Finally,
 * the outter block takes care of the exception.
 *
 */
TEST_CASE{

    volatile bool is_uncaught = false;

    TRY {

        TRY {

            THROW(RuntimeException, "You can't catch me!");

        } FINALLY {

            is_uncaught = e4c_is_uncaught();
        }

    } CATCH(RuntimeException) {

        TEST_DUMP("%s", e4c_get_exception()->message);
    }

    TEST_ASSERT(is_uncaught);
}
