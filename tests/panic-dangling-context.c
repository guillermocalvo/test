
# include <signal.h>
# include "testing.h"

static void failure(int _);

/**
 * Force panic due to dangling context.
 */
TEST_CASE{

    signal(SIGABRT, failure);

    TRY {
      goto oops;
    }

    oops:
    printf("Oh oh...");
}

static void failure(int _) {
    exit(EXIT_FAILURE);
}
