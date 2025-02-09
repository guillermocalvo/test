
# include "testing.h"

static const struct e4c_exception_type CAUSE = {NULL, "Cause"};
static const struct e4c_exception_type SUPPRESSED = {NULL, "Suppressed"};
static const struct e4c_exception_type OOPS = {NULL, "Oops"};

/**
 * Suppressed exception
 */
TEST_CASE{

    volatile bool caught1 = false;
    volatile bool caught2 = false;

    TRY {
        TRY {
            TRY {
                THROW(CAUSE, NULL);
            } FINALLY {
                THROW(SUPPRESSED, NULL);
            }
        } CATCH (SUPPRESSED) {
            caught1 = true;
            TEST_ASSERT(e4c_get_exception()->cause != NULL && e4c_get_exception()->cause->type == &CAUSE);
        } FINALLY {
            THROW(OOPS, NULL);
        }
    } CATCH (OOPS) {
        caught2 = true;
        TEST_ASSERT(e4c_get_exception()->cause == NULL);
    }

    TEST_ASSERT(caught1);
    TEST_ASSERT(caught2);
}
