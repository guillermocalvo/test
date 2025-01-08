
# include <signal.h>
# include "testing.h"


E4C_DEFINE_EXCEPTION(AbortException, "Abort exception.", RuntimeException);

void throw_on_signal(int);

/**
 * Catching `AbortException`
 *
 * This test calls `abort`; the library signal handling is enabled. The library
 * will convert the signal `SIGABRT` into the exception `AbortException`.
 * There is a `catch(AbortException)` block, therefore the exception will be
 * caught.
 *
 * This functionality relies on the platform's ability to handle signal
 * `SIGABRT`.
 *
 */
TEST_CASE{

    volatile bool caught = false;

    signal(SIGABRT, throw_on_signal);

    e4c_context_begin();

    E4C_TRY{

        abort();

        TEST_FAIL("AbortException should have been thrown");

    }E4C_CATCH(RuntimeException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &AbortException);
    }

    e4c_context_end();

    TEST_ASSERT(caught);
}

void throw_on_signal(int _) {
    E4C_THROW(AbortException, NULL);
}
