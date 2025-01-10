
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

    TEST_EXPECTING(ArithmeticException);

    signal(SIGFPE, throw_on_signal);

    int divisor = zero(rand());

    TEST_DUMP("%d", integer);
    TEST_DUMP("%d", divisor);

    int result = integer / divisor;

    TEST_DUMP("%d", result);

    raise(SIGFPE);

    TEST_FAIL("ArithmeticException should have been thrown");
}

int zero(int dummy){

    return(dummy ? 0 : 1);
}

void throw_on_signal(int _) {
    E4C_THROW(ArithmeticException, NULL);
}
