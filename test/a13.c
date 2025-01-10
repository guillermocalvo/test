
# include "testing.h"


/**
 * `e4c_context_end` call without beginning
 *
 * This test uses the library improperly, by attempting to call
 * `e4c_context_end`, without calling `e4c_context_begin` first.
 *
 * The library must ignore the call.
 *
 */
TEST_CASE{

    e4c_context_end();
}
