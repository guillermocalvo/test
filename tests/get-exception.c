
# include "testing.h"

static const struct e4c_exception_type OOPS = {NULL, "Oops"};

/**
 * `e4c_get_exception` call without starting a new exception frame
 *
 * The library must return NULL.
 *
 */
TEST_CASE{

    TEST_ASSERT(e4c_get_exception() == NULL);

    TRY {

        TEST_ASSERT(e4c_get_exception() == NULL);

    } FINALLY {

        TEST_ASSERT(e4c_get_exception() == NULL);
    }

    TRY {

        THROW(OOPS, "Catch me");

    } CATCH (OOPS) {

        TEST_ASSERT(e4c_get_exception() != NULL);

    } FINALLY {

        TEST_ASSERT(e4c_get_exception() != NULL);
    }

    TRY {

        TRY {

            THROW(OOPS, "Catch me");

        } CATCH (OOPS) {

            TEST_ASSERT(e4c_get_exception() != NULL);

        } FINALLY {

            TEST_ASSERT(e4c_get_exception() != NULL);
        }

    } FINALLY {

        TEST_ASSERT(e4c_get_exception() == NULL);
    }
}
