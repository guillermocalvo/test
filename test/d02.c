
# include "testing.h"


void another_function(void);

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Uncaught exception, thrown from another function
 *
 * This test calls a function which throws an exception; there is no `catch`
 * block to handle it.
 *
 */
TEST_CASE{

    TEST_EXPECTING(RuntimeException);

    another_function();
}


void another_function(void){

    THROW(RuntimeException, "Nobody will catch me.");
}
