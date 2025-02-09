
# include "testing.h"

static struct e4c_context my_context = {0};
static struct e4c_context * my_supplier(void);

/**
 * Force panic due to null block.
 *
 */
TEST_CASE{

    e4c_set_context_supplier(my_supplier);

    (void) e4c_next(NULL, 0, NULL);
}

static struct e4c_context * my_supplier(void) {
    free(my_context._innermost_block);
    my_context._innermost_block = NULL;
    return &my_context;
}
