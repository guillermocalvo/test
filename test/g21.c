
# include <signal.h>
# include "testing.h"


/**
 * Catching `ProgramSignal1Exception`
 *
 * This test raises `SIGUSR1`; the library signal handling is enabled; the
 * exception `ProgramSignal1Exception` is caught and then the program exits.
 *
 * This functionality relies on the `platform's ability to handle signals.
 *
 */
TEST_CASE{

#ifndef SIGUSR1

    TEST_SKIP("This platform does not support SIGUSR1");

#else

    volatile bool caught = false;

    e4c_context_begin(true);

    E4C_TRY{

        raise(SIGUSR1);

        TEST_FAIL("ProgramSignal1Exception should have been thrown");

    }E4C_CATCH(SignalException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &ProgramSignal1Exception);
    }

    e4c_context_end();

    TEST_ASSERT(caught);

#endif

}
