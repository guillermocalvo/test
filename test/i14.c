
# include "testing.h"


volatile bool custom_initializer_was_executed = false;
void * custom_initialize_handler(const struct e4c_exception * exception);


/**
 * Mixing custom initialization handler and formatted message
 *
 * This test sets custom *initialization handler* and *throws* an exception with
 * a formatted message.
 *
 */
TEST_CASE{

    e4c_get_current_context()->initialize_handler = custom_initialize_handler;

    TRY {

        THROW(RuntimeException, "%s_%s", "FORMATTED", "MESSAGE");


    } CATCH(RuntimeException) {

        TEST_ASSERT_STRING_EQUALS(e4c_get_exception()->message, "FORMATTED_MESSAGE");
    }

    TEST_ASSERT(custom_initializer_was_executed);

}

void * custom_initialize_handler(const struct e4c_exception * exception){

    custom_initializer_was_executed = true;

    return(NULL);
}
