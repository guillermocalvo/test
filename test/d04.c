
# include "testing.h"


void another_function(void);

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Uncaught exception, thrown from another function, called from `try` block
 *
 * This test starts a `try` block and then calls a function, which throws an
 * exception; there is no `catch` block to handle it.
 *
 */
TEST_CASE{

    TEST_EXPECTING(RuntimeException);

    TRY {

        another_function();
    }
}

void another_function(void){

    THROW(RuntimeException, "Nobody will catch me.");
}
