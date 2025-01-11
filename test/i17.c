
# include "testing.h"


static const struct e4c_exception_type CustomException = {&RuntimeException, "This is a custom exception"};


/**
 * Finding the cause of the cause
 *
 * This test *throws* an exception from two nested `catch` blocks. The
 * outtermost `catch` block inspects the *cause of the cause*.
 *
 */
TEST_CASE{

    TEST_EXPECTING(RuntimeException);

    TRY {

        TRY {

            TRY {

                THROW(CustomException, "This is the original cause of the issue");

            } CATCH(CustomException) {

                THROW(NullPointerException, "First wrapper");
            }

        } CATCH(NullPointerException) {

            THROW(RuntimeException, "Second wrapper");
        }

    } CATCH(RuntimeException) {

        const struct e4c_exception * exception = e4c_get_exception();

        TEST_ASSERT_EQUALS(exception->cause->type, &NullPointerException);
        TEST_ASSERT_EQUALS(exception->cause->cause->type, &CustomException);

        e4c_print_exception(exception);
    }
}
