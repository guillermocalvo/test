
# include <signal.h>
# include "testing.h"

static void failure(int _);

static const struct e4c_exception_type OOPS = {NULL, "Oops"};

/**
 * Force panic on reacquire.
 */
TEST_CASE{

    signal(SIGABRT, failure);

    REACQUIRE(100, OOPS, "Oh oh...");
}

static void failure(int _) {
    exit(EXIT_FAILURE);
}
