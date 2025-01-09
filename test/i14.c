
# include "testing.h"


volatile bool custom_initializer_was_executed = false;
void * custom_initialize_handler(const e4c_exception * exception);


/**
 * Mixing custom initialization handler and formatted message
 *
 * This test sets custom *initialization handler* and *throws* an exception with
 * a formatted message.
 *
 */
TEST_CASE{

    e4c_context_begin();

    e4c_context_set_handlers(NULL, NULL, custom_initialize_handler, NULL);

    E4C_TRY{

        E4C_THROW(RuntimeException, "%s_%s", "FORMATTED", "MESSAGE");


    }E4C_CATCH(RuntimeException){

        TEST_ASSERT_STRING_EQUALS(e4c_get_exception()->message, "FORMATTED_MESSAGE");
    }

    e4c_context_end();

    TEST_ASSERT(custom_initializer_was_executed);

}

void * custom_initialize_handler(const e4c_exception * exception){

    custom_initializer_was_executed = true;

    return(NULL);
}
