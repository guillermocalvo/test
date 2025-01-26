
#include <exceptions4c-lite.h>


struct e4c_context exceptions4c = {0};
const e4c_exception_type PROBLEM = "A problem happened.";


/**
 * Cleanup
 */
int main(void) {

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

    return !created || !started || finished || !destroyed;
}
