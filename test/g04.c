
# include <signal.h>
# include "testing.h"

void * null(int dummy);
void throw_npe(int);

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
static const struct e4c_exception_type NullPointerException = {&RuntimeException, "Null pointer."};

int integer = 123;

/**
 * Bad pointer exception
 *
 * This test attempts to dereference a null pointer; the library signal handling
 * is enabled. The library will convert the signal `SIGSEGV` into the exception
 * `NullPointerException`. There is no `catch` block, therefore the program will
 * terminate because of the uncaught exception.
 *
 * This functionality relies on the platform's ability to handle signal
 * `SIGSEGV`.
 *
 */
TEST_CASE{

    int * pointer = &integer;

    signal(SIGSEGV, throw_npe);

    TEST_EXPECTING(NullPointerException);

    pointer = null(integer);
    integer = *pointer;

    TEST_DUMP("%d", integer);
    TEST_DUMP("%p", (void *)pointer);
}

void * null(int dummy){

    return(dummy ? NULL : &integer);
}

void throw_npe(int _) {
    THROW(NullPointerException, NULL);
}
