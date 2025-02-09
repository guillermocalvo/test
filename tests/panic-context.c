
# include "testing.h"

static struct e4c_context * my_supplier(void);

/**
 * Force panic due to null context.
 *
 */
TEST_CASE{

    e4c_set_context_supplier(my_supplier);

    TRY {
    }
}

static struct e4c_context * my_supplier(void) {
  return NULL;
}
