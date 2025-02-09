

# include "testing.h"

void aux1(void);
void aux2(void);
void aux3(void);
void aux4(void);
void aux5(void);

static const struct e4c_exception_type OOPS = {NULL, "Oops"};

volatile bool finalized1 = false;
volatile bool finalized2 = false;

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

    } CATCH (OOPS) {

        caught = true;
    }

    TEST_ASSERT(caught);
    TEST_ASSERT(finalized1);
    TEST_ASSERT(finalized2);
}


void aux1(void){

    aux2();

    exit(EXIT_FAILURE);
}

void aux2(void){

    TRY {

        aux3();

    } FINALLY {

        finalized1 = true;
    }

    exit(EXIT_FAILURE);
}

void aux3(void){

    aux4();
}

void aux4(void){

    TRY {

        aux5();

    } FINALLY {

        finalized2 = true;
    }

    exit(EXIT_FAILURE);
}

void aux5(void){

    THROW(OOPS, NULL);
}
