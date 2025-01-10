
# include <signal.h>
# include "testing.h"


E4C_DEFINE_EXCEPTION(TerminationException, "Termination exception.", RuntimeException);

void throw_on_signal(int);

/**
 * Catching `TerminationException`
 *
 * This test raises `SIGTERM`; the library signal handling is enabled; the
 * exception `TerminationException` is caught and then the program exits.
 *
 * This functionality relies on the platform's ability to handle signals.
 *
 */
TEST_CASE{

    volatile bool caught = false;

    signal(SIGTERM, throw_on_signal);

    E4C_TRY{

        raise(SIGTERM);

        TEST_FAIL("TerminationException should have been thrown");

    }E4C_CATCH(RuntimeException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &TerminationException);
    }

    TEST_ASSERT(caught);
}

void throw_on_signal(int _) {
    E4C_THROW(TerminationException, NULL);
}
