
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
static const struct e4c_exception_type CustomException = {&RuntimeException, "This is a custom exception"};

/**
 * Finding the cause of an exception
 *
 * This test *throws* an exception from an inner `catch` block. An outter
 * `catch` block inspects the *cause*.
 *
 */
TEST_CASE{

    TRY {

        TRY {

            THROW(CustomException, "This is the original cause of the issue");

        } CATCH (RuntimeException) {

            THROW(RuntimeException, "This is the wrapper exception");
        }

    } CATCH (RuntimeException) {

        TEST_ASSERT_EQUALS(e4c_get_exception()->cause->type, &CustomException);
    }
}
