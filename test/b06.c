
# include "testing.h"


/**
 * `e4c_context_end` call after having already ended
 *
 * This test uses the library in an inconsistent way, by attempting to call
 * `e4c_context_end` twice in a row.
 *
 * The library must ignore the call.
 *
 */
TEST_CASE{

    e4c_context_begin();

    e4c_context_end();

    e4c_context_end();
}
