
# include <signal.h>
# include "testing.h"


/**
 * Catching `SignalAlarmException`
 *
 * This test raises `SIGALRM`; the library signal handling is enabled; the
 * exception `SignalAlarmException` is caught and then the program exits.
 *
 * This functionality relies on the platform's ability to handle signals.
 *
 */
TEST_CASE{

#ifndef SIGALRM

    TEST_SKIP("This platform does not support SIGALRM");

#else

    volatile bool caught = false;

    e4c_context_begin(true);

    E4C_TRY{

        raise(SIGALRM);

        TEST_FAIL("SignalAlarmException should have been thrown");

    }E4C_CATCH(SignalException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &SignalAlarmException);
    }

    e4c_context_end();

    TEST_ASSERT(caught);

#endif

}
