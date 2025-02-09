
# include "testing.h"

static const struct e4c_exception_type OOPS = {NULL, "Oops"};

/**
 * `CATCH_ALL`
 */
TEST_CASE{

    volatile bool caught1 = false;
    volatile bool caught2 = false;

    TRY {
        THROW(OOPS, NULL);
    } CATCH_ALL {
        caught1 = true;
    }

    TEST_ASSERT(caught1);

    TRY {
        const struct e4c_exception_type * NULL_EXCEPTION_TYPE = NULL;
        THROW(*NULL_EXCEPTION_TYPE, NULL);
    } CATCH_ALL {
        caught2 = true;
    }

    TEST_ASSERT(caught2);
}
