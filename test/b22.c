
# include "testing.h"
# include <signal.h>

/**
 * Throwing `NULL`
 *
 * This test passes `NULL` to a `throw` statement.
 *
 * The library must panic.
 *
 */
TEST_CASE{

    TRY {

        THROW( *( (const struct e4c_exception_type *)NULL ), "I see what you did there..." );

        TEST_X_FAIL("Expecting library panic");
    }
}
