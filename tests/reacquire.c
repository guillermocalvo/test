
# include "testing.h"


# define DISPOSE_FOO(IGNORE) 0

static const struct e4c_exception_type OOPS = {NULL, "Oops"};
static const struct e4c_exception_type GIVEUP = {NULL, "Giving up"};

/**
 * Reacquiring a resource
 *
 * This test tries to *acquire* a dummy resource and throws an exception. Then
 * the `catch` block tries to `reacquire` the resource up to three times. The
 * third reacquisition succeeds and then the resource is first *used* and then
 * *disposed*.
 *
 */
TEST_CASE{

    volatile int foo          = 0;
    volatile int total_acquisitions = 0;

    TRY {
        WITH(foo, DISPOSE_FOO) {

            total_acquisitions++;

            if (total_acquisitions == 1) {

                TEST_ECHO("First acquisition");

            } else {

                int reacquisitions = total_acquisitions - 1;

                TEST_DUMP("%d", reacquisitions);
            }

            THROW(OOPS, "Simulates an error while acquiring foo");

        } USE {

            TEST_DUMP("%d", foo);

        } CATCH (OOPS) {

            REACQUIRE(2, GIVEUP, NULL);
        }

    } CATCH (GIVEUP) {

        TEST_DUMP("%d", total_acquisitions);
    }

    TEST_ASSERT_EQUALS(total_acquisitions, 3);
}
