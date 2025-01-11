
# include <signal.h>
# include "testing.h"


static const struct e4c_exception_type AbortException = {&RuntimeException, "Abort exception."};

void throw_on_signal(int);

/**
 * Abort exception
 *
 * This test calls `abort`; the library signal handling is enabled. The library
 * will convert the signal `SIGABRT` into the exception `AbortException`.
 * There is no `catch` block, therefore the program will terminate because of
 * the uncaught exception.
 *
 * This functionality relies on the platform's ability to handle signal
 * `SIGABRT`.
 *
 */
TEST_CASE{

    TEST_EXPECTING(AbortException);

    signal(SIGABRT, throw_on_signal);

    abort();
}

void throw_on_signal(int _) {
    THROW(AbortException, NULL);
}
