
# include "testing.h"


char foobar[64] = "FOOBAR";
void custom_finalize_handler(void * custom_data);
volatile bool custom_handler_was_initialized = false;
volatile bool custom_handler_was_finalized = false;
static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Setting a custom finalization handler
 *
 * This test sets a custom *finalization handler*. Then *throws* an exception
 * and *catches* it.
 *
 */
TEST_CASE{

    struct e4c_context * context = e4c_get_context();
    context->custom_data = foobar;
    context->finalize_handler = custom_finalize_handler;

    TRY {

        THROW(RuntimeException, "Finalize my custom data");

    } CATCH(RuntimeException) {

        custom_handler_was_initialized = ( strcmp(e4c_get_exception()->custom_data, foobar) == 0 );
    }

    TEST_ASSERT(custom_handler_was_initialized);
    TEST_ASSERT(custom_handler_was_finalized);
}

void custom_finalize_handler(void * custom_data){

    custom_handler_was_finalized = true;
}
