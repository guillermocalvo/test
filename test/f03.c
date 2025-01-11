
# include "testing.h"


void aux(volatile bool * flag);


/**
 * Leaking a *parent* exception
 *
 * This test starts a `try` block, throws `RuntimeException` and attempts to
 * catch it with a `catch(NullPointerException)` block. This, obviously,
 * won't work, so the exception will be left uncaught.
 *
 */
TEST_CASE{

    volatile bool uncaught = false;

    TRY {

        aux(&uncaught);

    } CATCH(RuntimeException) {

        TEST_ASSERT( e4c_get_exception()->type == &RuntimeException );
    }

    TEST_ASSERT(uncaught);
}


void aux(volatile bool * flag){

    TRY {

        THROW(RuntimeException, "I am not an instance of NullPointerException.");

    } CATCH(NullPointerException) {

        TEST_FAIL("Block `catch(NullPointerException)` cannot handle a RuntimeException");

    } FINALLY {

        *flag = ( e4c_get_status() == e4c_failed );
    }

    THIS_SHOULD_NOT_HAPPEN;

    *flag = false;
}
