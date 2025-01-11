
# include <signal.h>
# include "testing.h"


static const struct e4c_exception_type UserInterruptionException = {&RuntimeException, "User interruption exception."};

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

    TRY {

        raise(SIGINT);

        TEST_FAIL("UserInterruptionException should have been thrown");

    } CATCH(RuntimeException) {

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &UserInterruptionException);
    }

    TEST_ASSERT(caught);
}

void throw_on_signal(int _) {
    THROW(UserInterruptionException, NULL);
}
