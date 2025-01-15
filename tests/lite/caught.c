
#include "testing.h"


const struct e4c_exception_type CustomException = {&RuntimeException, "Custom exception."};


/**
 * Caught exception
 */
 TEST_CASE{

    E4C_TRY{

        E4C_THROW(CustomException, "This is a custom exception");

    }E4C_CATCH(CustomException){

        printf("The custom exception was caught: %s\n", E4C_EXCEPTION.name);
    }
}
