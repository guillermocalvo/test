
# include "testing.h"


/**
 * `e4c_using_context` block after having already begun
 *
 * This test uses the library in an inconsistent way, by attempting to start a
 * `e4c_using_context` block when the exception context is already begun.
 *
 * The library must ignore the call.
 *
 */
TEST_CASE{

    e4c_context_begin();

    E4C_USING_CONTEXT {

        TEST_ECHO("This is fine");
    }

    e4c_context_end();
}
