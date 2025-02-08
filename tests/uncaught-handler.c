
# include "testing.h"


void custom_uncaught_handler(const struct e4c_exception * exception);

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Setting a custom uncaught handler
 *
 * This test sets a custom *uncaught handler*. Then *throws* an exception; there
 * is no `catch` block to handle it.
 *
 */
TEST_CASE{

    e4c_get_context()->uncaught_handler = custom_uncaught_handler;

    THROW(RuntimeException, "You can't stop me now!");
}

void custom_uncaught_handler(const struct e4c_exception * exception){

    exit(0);
}
