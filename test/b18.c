
# include "testing.h"


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

    E4C_RETRY(10, RuntimeException, NULL);
}
