
# include <signal.h>
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
static const struct e4c_exception_type AbortException = {&RuntimeException, "Abort exception."};

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

    TRY {

        abort();

        TEST_FAIL("AbortException should have been thrown");

    } CATCH(RuntimeException) {

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &AbortException);
    }

    TEST_ASSERT(caught);
}

void throw_on_signal(int _) {
    THROW(AbortException, NULL);
}
