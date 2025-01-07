
# include <signal.h>
# include "testing.h"


/**
 * Catching `SignalTrapException`
 *
 * This test raises `SIGTRAP`; the library signal handling is enabled; the
 * exception `SignalTrapException` is caught and then the program exits.
 *
 * This functionality relies on the platform's ability to handle signals.
 *
 */
TEST_CASE{

#ifndef SIGTRAP

    TEST_SKIP("This platform does not support SIGTRAP");

#else

    volatile bool exception_was_caught = false;

    e4c_context_begin(true);

    E4C_TRY{

        raise(SIGTRAP);

        TEST_FAIL("SignalTrapException should have been thrown");

    }E4C_CATCH(SignalException){

        exception_was_caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &SignalTrapException);
    }

    e4c_context_end();

    TEST_ASSERT(exception_was_caught);

#endif

}
