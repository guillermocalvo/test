
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

    TRY {

        THROW(NullPointerException, "I'm going to be caught by the first (generic) catch block.");

    } CATCH(RuntimeException) {

        caught1 = true;

    } CATCH(NullPointerException) {

        caught2 = true;
    }

    TEST_ASSERT(caught1);
    TEST_ASSERT(!caught2);
}
