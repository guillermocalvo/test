
# include "testing.h"


static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
static const struct e4c_exception_type NullPointerException = {&RuntimeException, "Null pointer."};
static const struct e4c_exception_type NotEnoughMemoryException = {&RuntimeException, "Not enough memory."};

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

    TRY {

        aux(&uncaught);

    } CATCH(RuntimeException) {

        TEST_ASSERT( e4c_get_exception()->type == &NullPointerException );
    }

    TEST_ASSERT(uncaught);
}


void aux(volatile bool * flag){

    TRY {

        THROW(NullPointerException, "I am not an instance of NotEnoughMemoryException.");

    } CATCH(NotEnoughMemoryException) {

        TEST_FAIL("Block `catch(NotEnoughMemoryException)` cannot handle an NullPointerException");

    } FINALLY {

        *flag = e4c_is_uncaught();
    }

    THIS_SHOULD_NOT_HAPPEN;

    *flag = false;
}
