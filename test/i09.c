
# include "testing.h"


/**
 * Getting status (succeeded)
 *
 * This test retrieves the completeness of a code block. There were no
 * exceptions during the `try` block, so the status is `e4c_succeeded`.
 *
 */
TEST_CASE{

    E4C_TRY{

        TEST_ASSERT( !e4c_get_exception() );

    }E4C_FINALLY{

        TEST_ASSERT_EQUALS(e4c_get_status(), e4c_succeeded);
    }
}
