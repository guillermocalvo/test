
# include "testing.h"


/**
 * Throwing an exception with a formatted message
 *
 * This test *throws* an exception an exception with a formatted message.
 */
TEST_CASE{

    E4C_TRY{

        E4C_THROW(RuntimeException, "%s_%s", "FORMATTED", "MESSAGE");

    }E4C_CATCH(RuntimeException){

        TEST_ASSERT_STRING_EQUALS(e4c_get_exception()->message, "FORMATTED_MESSAGE");
    }
}
