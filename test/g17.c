
# include <signal.h>
# include "testing.h"


/**
 * Catching `HangUpException`
 *
 * This test raises `SIGHUP`; the library signal handling is enabled; the
 * exception `HangUpException` is caught and then the program exits.
 *
 * This functionality relies on the platform's ability to handle signals.
 *
 */
TEST_CASE{

#ifndef SIGHUP

    TEST_SKIP("This platform does not support SIGHUP");

#else

    volatile bool caught = false;

    e4c_context_begin(true);

    E4C_TRY{

        raise(SIGHUP);

        TEST_FAIL("HangUpException should have been thrown");

    }E4C_CATCH(SignalException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &HangUpException);
    }

    e4c_context_end();

    TEST_ASSERT(caught);

#endif

}
