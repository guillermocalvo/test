
# include "testing.h"


char foobar[64] = "FOOBAR";
void custom_initializer(struct e4c_exception * exception);
volatile bool custom_handler_was_initialized = false;
static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Setting a custom initialization handler
 *
 * This test sets a custom *initialization handler*. Then *throws* an exception;
 * the `catch` block inspects the exception's *custom data*.
 *
 */
TEST_CASE{

    e4c_get_context()->initialize_exception = custom_initializer;

    TRY {

        THROW(RuntimeException, "Initialize my custom data with the result of a function");

    } CATCH(RuntimeException) {

        custom_handler_was_initialized = ( strcmp(e4c_get_exception()->data, foobar) == 0 );
    }

    TEST_ASSERT(custom_handler_was_initialized);
}

void custom_initializer(struct e4c_exception * exception){

    exception->data = &foobar;
}
