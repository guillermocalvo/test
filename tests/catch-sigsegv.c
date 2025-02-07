
# include <signal.h>
# include "testing.h"

void * null(int dummy);
void throw_on_signal(int);

static const struct e4c_exception_type NullPointerException = {NULL, "Null pointer."};

int integer = 123;

/**
 * Catching `NullPointerException`
 *
 * This test attempts to dereference a null pointer; the library signal handling
 * is enabled. The library will convert the signal `SIGSEGV` into the exception
 * `NullPointerException`. There is a `catch(NullPointerException)` block,
 * therefore the exception will be caught.
 *
 * This functionality relies on the platform's ability to handle signal
 * `SIGSEGV`.
 *
 */
TEST_CASE{

    volatile bool caught = false;

    signal(SIGSEGV, throw_on_signal);

    TRY {

        int * pointer = &integer;

        pointer = null(integer);
        integer = *pointer;

        TEST_FAIL("NullPointerException should have been thrown");

        TEST_DUMP("%d", integer);
        TEST_DUMP("%p", (void *)pointer);

    } CATCH (NullPointerException) {

        caught = true;

        TEST_ASSERT(e4c_get_exception()->type == &NullPointerException);
    }

    TEST_ASSERT(caught);
}

void * null(int dummy){

    return(dummy ? NULL : &integer);
}

void throw_on_signal(int _) {
    THROW(NullPointerException, NULL);
}
