
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * `reacquire` statement with no `with... use` block
 *
 * This test uses the library in an inconsistent way, by attempting to
 * `reacquire` without having entered a `with` block.
 *
 * The library must signal the misuse by throwing the exception
 * `ExceptionSystemFatalError`.
 *
 */
TEST_CASE{

    TEST_EXPECTING(ExceptionSystemFatalError);

    REACQUIRE(10, RuntimeException, NULL);
}
