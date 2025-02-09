
# include <signal.h>
# include "testing.h"

static struct e4c_context my_context = {0};
static struct e4c_context * my_supplier(void);
static void failure(int _);

/**
 * Force panic due to null block.
 *
 */
TEST_CASE{

    signal(SIGABRT, failure);

    e4c_set_context_supplier(my_supplier);

    (void) e4c_try(NULL, 0, NULL);
}

static struct e4c_context * my_supplier(void) {
    free(my_context._innermost_block);
    my_context._innermost_block = NULL;
    return &my_context;
}

static void failure(int _) {
    exit(EXIT_FAILURE);
}
