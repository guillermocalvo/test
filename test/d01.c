
# include "testing.h"


/**
 * Uncaught exception
 *
 * This test throws an exception; there is no `catch` block to handle it.
 *
 */
TEST_CASE{

    TEST_EXPECTING(RuntimeException);

    E4C_THROW(RuntimeException, "Nobody will catch me.");
}
