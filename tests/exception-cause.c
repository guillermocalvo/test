
# include "testing.h"

static const struct e4c_exception_type ERROR1 = {NULL, "ERROR 1"};
static const struct e4c_exception_type ERROR2 = {NULL, "ERROR 2"};
static const struct e4c_exception_type ERROR3 = {NULL, "ERROR 3"};

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

            THROW(ERROR1, "This is the original cause of the issue");

        } CATCH (ERROR1) {

            THROW(ERROR2, "This is the wrapper exception");
        }

    } CATCH (ERROR2) {

        TEST_ASSERT_EQUALS(e4c_get_exception()->cause->type, &ERROR1);
    }

    TRY {

        TRY {

            TRY {

                THROW(ERROR1, "This is the original cause of the issue");

            } CATCH (ERROR1) {

                THROW(ERROR2, "First wrapper");
            }

        } CATCH (ERROR2) {

            THROW(ERROR3, "Second wrapper");
        }

    } CATCH (ERROR3) {

        const struct e4c_exception * exception = e4c_get_exception();

        TEST_ASSERT_EQUALS(exception->cause->type, &ERROR2);
        TEST_ASSERT_EQUALS(exception->cause->cause->type, &ERROR1);

        fprintf(stderr, "CAUGHT: %s: %s\n", exception->name, exception->message);
        fprintf(stderr, "  CAUSED BY: %s: %s\n", exception->cause->name, exception->cause->message);
        fprintf(stderr, "    CAUSED BY: %s: %s\n", exception->cause->cause->name, exception->cause->cause->message);
    }
}
