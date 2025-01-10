
# include "testing.h"


/**
 * `e4c_get_status` call without starting a new exception frame
 *
 * This test uses the library improperly, by attempting to call
 * `e4c_get_status`, without starting a new exception frame.
 *
 * The library must abort if NDEBUG is undefined.
 *
 */
TEST_CASE{

    (void) e4c_get_status();
}
