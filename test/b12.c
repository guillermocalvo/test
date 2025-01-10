
# include "testing.h"


/**
 * `break` statement in the middle of a `e4c_using_context` block
 *
 * This test uses the library in an inconsistent way, by breaking out of a
 * `e4c_using_context` block.
 *
 */
TEST_CASE{

    E4C_USING_CONTEXT {

        TEST_ECHO("Inside `e4c_using_context` block...");

        /* Never jump out of a `using` block! */
        break;
    }
}
