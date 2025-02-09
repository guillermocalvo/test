
# include "testing.h"

static const struct e4c_exception_type GENERIC = {NULL, "Generic exception"};
static const struct e4c_exception_type SPECIFIC = {&GENERIC, "Specific exception"};
static const struct e4c_exception_type DIFFERENT = {&GENERIC, "Different exception"};
static const struct e4c_exception_type MORE_SPECIFIC = {&SPECIFIC, "More specific exception"};

/**
 * Catching a generic exception
 *
 * This test starts a `try` block, throws `NullPointerException` and catches
 * it with a `catch(RuntimeException)` block.
 *
 */
TEST_CASE{

    volatile bool caught1 = false;
    volatile bool caught2 = false;
    volatile bool caught3 = false;

    TRY {
        THROW(SPECIFIC, NULL);
    } CATCH (GENERIC) {
        caught1 = true;
    }

    TEST_ASSERT(caught1);

    TRY {
        TRY {
            THROW(SPECIFIC, NULL);
        } CATCH (DIFFERENT) {
            abort();
        } FINALLY {
            THROW(MORE_SPECIFIC, NULL);
        }
    } CATCH (GENERIC) {
        caught2 = true;
    }

    TEST_ASSERT(caught2);

    TRY {
        TRY {
            THROW(SPECIFIC, NULL);
        } CATCH (MORE_SPECIFIC) {
            abort();
        } FINALLY {
            THROW(MORE_SPECIFIC, NULL);
        }
    } CATCH (SPECIFIC) {
        caught3 = true;
    }

    TEST_ASSERT(caught3);
}
