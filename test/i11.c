
# include "testing.h"


/**
 * Getting status (failed)
 *
 * This test retrieves the completeness of a code block. There was an uncaught
 * exceptions during the `try` block, so the status is `e4c_failed`. Finally,
 * the outter block takes care of the exception.
 *
 */
TEST_CASE{

    volatile enum e4c_status status = e4c_succeeded;

    TRY {

        TRY {

            THROW(RuntimeException, "You can't catch me!");

        } FINALLY {

            status = e4c_get_status();
        }

    } CATCH(RuntimeException) {

        TEST_DUMP("%s", e4c_get_exception()->message);
    }

    TEST_ASSERT(status == e4c_failed);
}
