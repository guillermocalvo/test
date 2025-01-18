
#include "testing.h"


const struct e4c_exception_type my_exception = {NULL, "My exception."};


/**
 * Caught exception
 */
 TEST_CASE{

    TRY {

        THROW(my_exception, "This is my exception");

    } CATCH_ALL {

        printf("The exception was caught: %s\n", THROWN_EXCEPTION.name);
    }
}
