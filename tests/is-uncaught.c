
# include "testing.h"

static const struct e4c_exception_type OOPS = {NULL, "Oops"};

/**
 * `e4c_is_uncaught` call without starting a new exception frame
 *
 * This test uses the library improperly, by attempting to call
 * `e4c_is_uncaught`, without starting a new exception frame.
 *
 * The library must return false.
 *
 */
TEST_CASE{

    TEST_ASSERT(e4c_is_uncaught() == false);

    TRY {

        TEST_ASSERT(e4c_is_uncaught() == false);

    } FINALLY {

        TEST_ASSERT(e4c_is_uncaught() == false);
    }

    TRY {

        THROW(OOPS, "Catch me");

    } CATCH (OOPS) {

        TEST_ASSERT(e4c_is_uncaught() == false);

    } FINALLY {

        TEST_ASSERT(e4c_is_uncaught() == false);
    }

    TRY {

        TRY {

            THROW(OOPS, "Catch me");

        } FINALLY {

            TEST_ASSERT(e4c_is_uncaught() == true);
        }

    } CATCH (OOPS) {

        TEST_ASSERT(e4c_is_uncaught() == false);

    } FINALLY {

        TEST_ASSERT(e4c_is_uncaught() == false);
    }
}
