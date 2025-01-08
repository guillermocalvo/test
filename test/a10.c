
# include "testing.h"


/**
 * `e4c_frame_get_stage` call without beginning
 *
 * This test uses the library improperly, by attempting to call
 * `e4c_frame_get_stage`, without calling `e4c_context_begin` first.
 *
 * The library must signal the misuse by throwing the exception
 * `ContextHasNotBegunYet`.
 *
 */
TEST_CASE{

    TEST_EXPECTING(ContextHasNotBegunYet);

    /* This function must not be called like this! */
    (void) e4c_get_current_stage(E4C_DEBUG_INFO);
}
