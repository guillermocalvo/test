
#define NDEBUG

# include "testing.h"

static const struct e4c_exception_type OOPS = {NULL, "Oops"};

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

        THROW(OOPS, "Nobody will catch me.");
    }
}
