
# include <signal.h>
# include "testing.h"


static const struct e4c_exception_type TerminationException = {&RuntimeException, "Termination exception."};

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

    TRY {

        raise(SIGTERM);

        TEST_FAIL("TerminationException should have been thrown");

    } CATCH(RuntimeException) {

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &TerminationException);
    }

    TEST_ASSERT(caught);
}

void throw_on_signal(int _) {
    THROW(TerminationException, NULL);
}
