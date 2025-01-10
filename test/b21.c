
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
        e4c_restart(e4c_done + 1, 10, &RuntimeException, E4C_DEBUG_INFO, NULL);
    }

    e4c_context_end();
}
