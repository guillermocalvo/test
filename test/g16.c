
# include <signal.h>
# include "testing.h"


/**
 * Catching `BrokenPipeException`
 *
 * This test raises `SIGPIPE`; the library signal handling is enabled; the
 * exception `BrokenPipeException` is caught and then the program exits.
 *
 * This functionality relies on the platform's ability to handle signals.
 *
 */
TEST_CASE{

#ifndef SIGPIPE

    TEST_SKIP("This platform does not support SIGPIPE");

#else

    volatile bool caught = false;

    e4c_context_begin(true);

    E4C_TRY{

        raise(SIGPIPE);

        TEST_FAIL("BrokenPipeException should have been thrown");

    }E4C_CATCH(SignalException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &BrokenPipeException);
    }

    e4c_context_end();

    TEST_ASSERT(caught);

#endif

}
