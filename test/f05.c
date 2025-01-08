
# include "testing.h"


/**
 * Same `catch` block twice
 *
 * This test starts a `try` block, throws `RuntimeException` and attempts to
 * `catch` it twice with two `catch(RuntimeException)` blocks.
 *
 * The exception will only be caught by the first one.
 *
 */
TEST_CASE{

    volatile bool caught1 = false;
    volatile bool caught2 = false;

    e4c_context_begin();

    E4C_TRY{

        E4C_THROW(RuntimeException, "I can only be caught once for each try block.");

    }E4C_CATCH(RuntimeException){

        caught1 = true;

    }E4C_CATCH(RuntimeException){

        caught2 = true;
    }

    e4c_context_end();

    TEST_ASSERT(caught1);
    TEST_ASSERT(!caught2);
}
