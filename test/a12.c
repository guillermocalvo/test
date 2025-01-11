
# include "testing.h"


/**
 * `e4c_is_uncaught` call without starting a new exception frame
 *
 * This test uses the library improperly, by attempting to call
 * `e4c_is_uncaught`, without starting a new exception frame.
 *
 * The library must return false.
 *
 */
TEST_CASE{

    TEST_ASSERT(!e4c_is_uncaught());
}
