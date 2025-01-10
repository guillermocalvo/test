
# include "testing.h"


static E4C_DEFINE_EXCEPTION(NotEnoughMemoryException, "Not enough memory.", RuntimeException);

void aux(volatile bool * flag);


/**
 * Leaking a *sibling* exception
 *
 * This test starts a `try` block, throws `NullPointerException` and
 * attempts to catch it with a `catch(NotEnoughMemoryException)` block. This,
 * obviously, won't work, so the exception will be left uncaught.
 *
 */
TEST_CASE{

    volatile bool uncaught = false;

    TEST_EXPECTING(RuntimeException);

    e4c_context_begin();

    E4C_TRY{

        aux(&uncaught);

    }E4C_CATCH(RuntimeException){

        TEST_ASSERT( e4c_get_exception()->type == &NullPointerException );
    }

    TEST_ASSERT(uncaught);

    e4c_context_end();
}


void aux(volatile bool * flag){

    E4C_TRY{

        E4C_THROW(NullPointerException, "I am not an instance of NotEnoughMemoryException.");

    }E4C_CATCH(NotEnoughMemoryException){

        TEST_FAIL("Block `catch(NotEnoughMemoryException)` cannot handle an NullPointerException");

    }E4C_FINALLY{

        *flag = ( e4c_get_status() == e4c_failed );
    }

    THIS_SHOULD_NOT_HAPPEN;

    *flag = false;
}
