
# include <signal.h>
# include "testing.h"

static struct e4c_context * my_supplier(void);
static void failure(int _);

/**
 * Force panic due to null context.
 *
 */
TEST_CASE{

    signal(SIGABRT, failure);

    e4c_set_context_supplier(my_supplier);

    TRY {
    }
}

static struct e4c_context * my_supplier(void) {
  return NULL;
}

static void failure(int _) {
    exit(EXIT_FAILURE);
}
