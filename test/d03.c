
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

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
