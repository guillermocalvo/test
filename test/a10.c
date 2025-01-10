
# include "testing.h"


/**
 * `e4c_frame_get_stage` call without starting a new exception frame
 *
 * This test uses the library improperly, by attempting to call
 * `e4c_get_current_stage`, without starting a new exception frame.
 *
 * The library must abort if NDEBUG is undefined.
 *
 */
TEST_CASE{

    /* This function must not be called like this! */
    (void) e4c_get_current_stage();
}
