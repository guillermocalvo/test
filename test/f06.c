
# include "testing.h"


/**
 * Only one `catch` block will handle the exception
 *
 * This test starts a `try` block, throws `NullPointerException` and
 * attempts to catch it with a `catch(NullPointerException)` block, but
 * there is a previous `catch(RuntimeException)` block which will eventually
 * handle it.
 *
 */
TEST_CASE{

    volatile bool caught1 = false;
    volatile bool caught2 = false;

    e4c_context_begin();

    E4C_TRY{

        E4C_THROW(NullPointerException, "I'm going to be caught by the first (generic) catch block.");

    }E4C_CATCH(RuntimeException){

        caught1 = true;

    }E4C_CATCH(NullPointerException){

        caught2 = true;
    }

    e4c_context_end();

    TEST_ASSERT(caught1);
    TEST_ASSERT(!caught2);
}
