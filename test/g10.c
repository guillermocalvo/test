
# include <signal.h>
# include "testing.h"


E4C_DEFINE_EXCEPTION(IllegalInstructionException, "Illegal instruction exception.", RuntimeException);

void throw_on_signal(int);


/**
 * Catching `IllegalInstructionException`
 *
 * This test raises `SIGILL`; the library signal handling is enabled; the
 * exception `IllegalInstructionException` is caught and then the program exits.
 *
 * This functionality relies on the platform's ability to handle signals.
 *
 */
TEST_CASE{

    volatile bool exception_was_caught = false;

    signal(SIGILL, throw_on_signal);

    e4c_context_begin();

    E4C_TRY{

        raise(SIGILL);

        TEST_FAIL("IllegalInstructionException should have been thrown");

    }E4C_CATCH(RuntimeException){

        exception_was_caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &IllegalInstructionException);
    }

    e4c_context_end();

    TEST_ASSERT(exception_was_caught);
}

void throw_on_signal(int _) {
    E4C_THROW(IllegalInstructionException, NULL);
}
