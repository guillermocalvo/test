
# include <signal.h>
# include "testing.h"


/**
 * Catching `CPUTimeException`
 *
 * This test raises `SIGXCPU`; the library signal handling is enabled; the
 * exception `CPUTimeException` is caught and then the program exits.
 *
 * This functionality relies on the platform's ability to handle signals.
 *
 */
TEST_CASE{

#ifndef SIGXCPU

    TEST_SKIP("This platform does not support SIGXCPU");

#else

    volatile bool caught = false;

    e4c_context_begin(true);

    E4C_TRY{

        raise(SIGXCPU);

        TEST_FAIL("CPUTimeException should have been thrown");

    }E4C_CATCH(SignalException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &CPUTimeException);
    }

    e4c_context_end();

    TEST_ASSERT(caught);

#endif

}
