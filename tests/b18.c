
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * `retry` statement with no `try` block
 *
 * This test uses the library in an inconsistent way, by attempting to `retry`
 * without having entered a `try` block.
 *
 * The library must panic.
 *
 */
TEST_CASE{

    RETRY(10, RuntimeException, NULL);
}
