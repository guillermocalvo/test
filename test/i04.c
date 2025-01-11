
# include "testing.h"


/**
 * Throwing an exception with a formatted message
 *
 * This test *throws* an exception an exception with a formatted message.
 */
TEST_CASE{

    TRY {

        THROW(RuntimeException, "%s_%s", "FORMATTED", "MESSAGE");

    } CATCH(RuntimeException) {

        TEST_ASSERT_STRING_EQUALS(e4c_get_exception()->message, "FORMATTED_MESSAGE");
    }
}
