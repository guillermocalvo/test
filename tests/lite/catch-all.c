
#include "testing.h"


const struct e4c_exception_type MY_EXCEPTION = {NULL, "My exception."};


/**
 * Caught exception
 */
 TEST_CASE{

    TRY {

        THROW(MY_EXCEPTION, "This is my exception");

    } CATCH_ALL {

        printf("The exception was caught: %s\n", THROWN_EXCEPTION.name);
    }
}
