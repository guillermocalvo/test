
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * `CATCH_ALL`
 */
TEST_CASE{

    volatile bool caught = false;

    TRY {

        THROW(RuntimeException, NULL);

    } CATCH_ALL {

        caught = true;
    }

    TEST_ASSERT(caught);
}
