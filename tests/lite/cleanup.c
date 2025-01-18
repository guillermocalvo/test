
#include "testing.h"


const struct e4c_exception_type PROBLEM = {NULL, "A problem happened."};


/**
 * Cleanup
 */
TEST_CASE{

    volatile int created     = 0;
    volatile int destroyed   = 0;
    volatile int started     = 0;
    volatile int finished    = 0;

    TRY {

        created = 1;

        TRY {

            started = 1;

            THROW(PROBLEM, "Get me out of here");

            finished = 1; /* this should not happen */

        } FINALLY {

            destroyed = 1;
        }

    } CATCH(PROBLEM) {

        printf("No problem :-)");
    }

    TEST_ASSERT(created);
    TEST_ASSERT(started);
    TEST_ASSERT(!finished);
    TEST_ASSERT(destroyed);
}
