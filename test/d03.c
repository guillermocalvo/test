
# include "testing.h"


/**
 * Uncaught exception, thrown from `try` block
 *
 * This test starts a `try` block and then throws an exception; there is no
 * `catch` block to handle it.
 *
 */
TEST_CASE{

    TEST_EXPECTING(RuntimeException);

    TRY {

        THROW(RuntimeException, "Nobody will catch me.");
    }
}
