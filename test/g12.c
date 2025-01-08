
# include <signal.h>
# include "testing.h"


E4C_DEFINE_EXCEPTION(UserInterruptionException, "User interruption exception.", RuntimeException);

void throw_on_signal(int);

/**
 * Catching `UserInterruptionException`
 *
 * This test raises `SIGINT`; the library signal handling is enabled; the
 * exception `UserInterruptionException` is caught and then the program exits.
 *
 * This functionality relies on the platform's ability to handle signals.
 *
 */
TEST_CASE{

    volatile bool caught = false;

    signal(SIGINT, throw_on_signal);

    e4c_context_begin();

    E4C_TRY{

        raise(SIGINT);

        TEST_FAIL("UserInterruptionException should have been thrown");

    }E4C_CATCH(RuntimeException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &UserInterruptionException);
    }

    e4c_context_end();

    TEST_ASSERT(caught);
}

void throw_on_signal(int _) {
    E4C_THROW(UserInterruptionException, NULL);
}
