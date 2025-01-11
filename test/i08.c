
# include "testing.h"


char foobar[64] = "FOOBAR";
void custom_finalize_handler(void * custom_data);
volatile bool custom_handler_was_initialized = false;
volatile bool custom_handler_was_finalized = false;


/**
 * Setting a custom finalization handler
 *
 * This test sets a custom *finalization handler*. Then *throws* an exception
 * and *catches* it.
 *
 */
TEST_CASE{

    struct e4c_context * context = e4c_get_current_context();
    context->custom_data = foobar;
    context->finalize_handler = custom_finalize_handler;

    E4C_TRY{

        E4C_THROW(RuntimeException, "Finalize my custom data");

    }E4C_CATCH(RuntimeException){

        custom_handler_was_initialized = ( strcmp(e4c_get_exception()->custom_data, foobar) == 0 );
    }

    TEST_ASSERT(custom_handler_was_initialized);
    TEST_ASSERT(custom_handler_was_finalized);
}

void custom_finalize_handler(void * custom_data){

    custom_handler_was_finalized = true;
}
