
# include "testing.h"


char foobar[64] = "FOOBAR";
void * custom_initialize_handler(const struct e4c_exception * exception);
volatile bool custom_handler_was_initialized = false;


/**
 * Setting a custom initialization handler
 *
 * This test sets a custom *initialization handler*. Then *throws* an exception;
 * the `catch` block inspects the exception's *custom data*.
 *
 */
TEST_CASE{

    e4c_get_current_context()->initialize_handler = custom_initialize_handler;

    TRY {

        THROW(RuntimeException, "Initialize my custom data with the result of a function");

    } CATCH(RuntimeException) {

        custom_handler_was_initialized = ( strcmp(e4c_get_exception()->custom_data, foobar) == 0 );
    }

    TEST_ASSERT(custom_handler_was_initialized);
}

void * custom_initialize_handler(const struct e4c_exception * exception){

    return(&foobar);
}
