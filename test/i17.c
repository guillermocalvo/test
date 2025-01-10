
# include "testing.h"


E4C_DEFINE_EXCEPTION(CustomException, "This is a custom exception", RuntimeException);


/**
 * Finding the cause of the cause
 *
 * This test *throws* an exception from two nested `catch` blocks. The
 * outtermost `catch` block inspects the *cause of the cause*.
 *
 */
TEST_CASE{

    TEST_EXPECTING(RuntimeException);

    E4C_TRY {

        E4C_TRY {

            E4C_TRY {

                E4C_THROW(CustomException, "This is the original cause of the issue");

            } E4C_CATCH(CustomException) {

                E4C_THROW(NullPointerException, "First wrapper");
            }

        } E4C_CATCH(NullPointerException) {

            E4C_THROW(RuntimeException, "Second wrapper");
        }

    } E4C_CATCH(RuntimeException) {

        const struct e4c_exception * exception = e4c_get_exception();

        TEST_ASSERT_EQUALS(exception->cause->type, &NullPointerException);
        TEST_ASSERT_EQUALS(exception->cause->cause->type, &CustomException);

        e4c_print_exception(exception);
    }
}
