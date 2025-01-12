
# include <signal.h>
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
static const struct e4c_exception_type IllegalInstructionException = {&RuntimeException, "Illegal instruction exception."};

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

    TRY {

        raise(SIGILL);

        TEST_FAIL("IllegalInstructionException should have been thrown");

    } CATCH(RuntimeException) {

        exception_was_caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &IllegalInstructionException);
    }

    TEST_ASSERT(exception_was_caught);
}

void throw_on_signal(int _) {
    THROW(IllegalInstructionException, NULL);
}
