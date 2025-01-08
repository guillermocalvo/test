
# include <signal.h>
# include "testing.h"


E4C_DEFINE_EXCEPTION(ArithmeticException, "Arithmetic exception.", RuntimeException);

int zero(int dummy);
void throw_on_signal(int);
int integer = 123;


/**
 * Division by zero exception
 *
 * This test attempts to divide by zero; the library signal handling is enabled.
 * The library will convert the signal `SIGFPE` into the exception
 * `ArithmeticException`. There is no `catch` block, therefore the program will
 * terminate because of the uncaught exception.
 *
 * This functionality relies on the platform's ability to handle signal
 * `SIGFPE`.
 *
 */
TEST_CASE{

    TEST_SKIP("Skip this test temporarily");

    signal(SIGFPE, throw_on_signal);

    int divisor = 10;

    TEST_EXPECTING(ArithmeticException);

    e4c_context_begin();

    divisor = zero(integer);
    integer = integer / divisor;

    e4c_context_end();

    TEST_DUMP("%d", integer);
    TEST_DUMP("%d", divisor);
}

int zero(int dummy){

    return(dummy ? 0 : 1);
}

void throw_on_signal(int _) {
    E4C_THROW(ArithmeticException, NULL);
}
