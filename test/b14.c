
# include "testing.h"


/**
 * `e4c_using_context` block after having already begun
 *
 * This test uses the library in an inconsistent way, by attempting to start a
 * `e4c_using_context` block when the exception context is already begun.
 *
 * The library must signal the misuse by throwing the exception
 * `ContextAlreadyBegun`.
 *
 */
TEST_CASE{

    TEST_EXPECTING(ContextAlreadyBegun);

    e4c_context_begin(false);

    e4c_using_context(false){

        THIS_SHOULD_NOT_HAPPEN;
    }

    e4c_context_end();
}
