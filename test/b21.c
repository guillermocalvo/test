
# include "testing.h"
# include <signal.h>


/**
 * Repeating an invalid stage
 *
 * This test tries to repeat an invalid stage.
 *
 * The library must signal the misuse by throwing the exception
 * `ExceptionSystemFatalError`.
 *
 */
TEST_CASE{

    TEST_EXPECTING(ExceptionSystemFatalError);

    e4c_context_begin();

    E4C_TRY {

        /* Never call this function like this! */
        e4c_restart(10, e4c_done + 1, E4C_DEBUG_INFO);
    }

    e4c_context_end();
}
