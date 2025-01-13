
# include "testing.h"


/**
 * `e4c_try` call without starting a new exception frame
 *
 * This test uses the library improperly, by attempting to call
 * `e4c_try`, without starting a new exception frame.
 *
 * The library must abort if NDEBUG is undefined.
 *
 */
TEST_CASE{

    /* This function must not be called like this! */
    (void) e4c_try(__FILE__, __LINE__, __func__);
}
