
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Uncaught exception
 *
 * This test throws an exception; there is no `catch` block to handle it.
 *
 */
TEST_CASE{

    TEST_EXPECTING(RuntimeException);

    THROW(RuntimeException, "Nobody will catch me.");
}
