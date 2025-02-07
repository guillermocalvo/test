
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Getting status (recovered)
 *
 * This test retrieves the completeness of a code block. There was an exceptions
 * during the `try` block but it was *caught*, so the status is `e4c_recovered`.
 *
 */
TEST_CASE{

    TRY {

        THROW(RuntimeException, "Please catch me");

    } CATCH (RuntimeException) {

        TEST_ASSERT( e4c_get_exception() );

    } FINALLY {

        TEST_ASSERT(e4c_get_exception() != NULL && !e4c_is_uncaught());
    }
}
