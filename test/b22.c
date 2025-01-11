
# include "testing.h"
# include <signal.h>


/**
 * Throwing `NULL`
 *
 * This test passes `NULL` to a `throw` statement.
 *
 * The library must throw `NullPointerException` instead.
 *
 */
TEST_CASE{

    volatile bool thrown = false;
    volatile bool caught = false;

    TEST_EXPECTING(NullPointerException);

    TRY {

        thrown = true;

        THROW( *( (const struct e4c_exception_type *)NULL ), "I see what you did there..." );

        thrown = false;

    } CATCH(RuntimeException) {

        caught = (e4c_get_exception()->type == &NullPointerException);
    }

    TEST_ASSERT(thrown);
    TEST_ASSERT(caught);
}
