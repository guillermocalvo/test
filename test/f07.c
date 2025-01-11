
# include "testing.h"


void aux1(void);
void aux2(void);
void aux3(void);
void aux4(void);
void aux5(void);


/**
 * Catching an exception thrown deep down the call stack
 *
 * This test starts a `try` block, and calls a function. That function calls
 * another one, and so on. Eventually, one of those functions will throw an
 * exception that will be caught by a `catch(RuntimeException)` right next to
 * the `try` block.",
 *
 */
TEST_CASE{

    volatile bool caught = false;

    TRY {

        aux1();

    } CATCH(RuntimeException) {

        caught = true;

        TEST_ASSERT_EQUALS(e4c_get_exception()->type, &RuntimeException);
    }

    TEST_ASSERT(caught);
}


void aux1(void){

    aux2();
}

void aux2(void){

    TRY {

        aux3();

    } FINALLY {

        /* The exception has not been caught yet */
        TEST_ASSERT_EQUALS(e4c_get_status(), e4c_failed);
    }
}

void aux3(void){

    aux4();
}

void aux4(void){

    TRY {

        aux5();

    } CATCH(NullPointerException) {

        TEST_FAIL("Block `catch(NullPointerException)` cannot handle a RuntimeException");
    }
}

void aux5(void){

    THROW(RuntimeException, "I'm going to be caught.");

    TEST_FAIL("RuntimeException should have been thrown");
}
