
# include <signal.h>
# include "testing.h"


E4C_DEFINE_EXCEPTION(ArithmeticException, "Arithmetic exception.", RuntimeException);

int zero(int dummy);
void throw_on_signal(int);
int integer = 123;


/**
 * Catching `ArithmeticException`
 *
 * This test attempts to divide by zero; the library signal handling is enabled.
 * The library will convert the signal `SIGFPE` into the exception
 * `ArithmeticException`. There is a `catch(SignalException)` block, therefore
 * the exception will be caught.
 *
 * This functionality relies on the platform's ability to handle signal
 * `SIGFPE`.
 *
 */
TEST_CASE{

    TEST_SKIP("Skip this test temporarily");

    signal(SIGFPE, throw_on_signal);

    volatile bool caught = false;

    e4c_context_begin();

    E4C_TRY{

        int divisor = 10;

        divisor = zero(integer);
        integer = integer / divisor;

        TEST_DUMP("%d", integer);

        TEST_FAIL("ArithmeticException should have been thrown");

    }E4C_CATCH(RuntimeException){

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &ArithmeticException);
    }

    e4c_context_end();

    TEST_ASSERT(caught);
}

int zero(int dummy){

    return(dummy ? 0 : 1);
}

void throw_on_signal(int _) {
    E4C_THROW(ArithmeticException, NULL);
}
