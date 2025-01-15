
# include "testing.h"


volatile bool custom_initializer_was_executed = false;
void custom_initializer(struct e4c_exception * exception);
static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Mixing custom initialization handler and formatted message
 *
 * This test sets custom *initialization handler* and *throws* an exception with
 * a formatted message.
 *
 */
TEST_CASE{

    e4c_get_context()->initialize_exception = custom_initializer;

    TRY {

        THROW(RuntimeException, "%s_%s", "FORMATTED", "MESSAGE");


    } CATCH(RuntimeException) {

        TEST_ASSERT_STRING_EQUALS(e4c_get_exception()->message, "FORMATTED_MESSAGE");
    }

    TEST_ASSERT(custom_initializer_was_executed);

}

void custom_initializer(struct e4c_exception * exception){

    custom_initializer_was_executed = true;

    TEST_ASSERT_EQUALS(exception->data, NULL);
}
