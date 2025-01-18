
#include "testing.h"


const struct e4c_exception_type COLOR_EXCEPTION = {NULL, "Color exception."};
const struct e4c_exception_type RED_EXCEPTION = {&COLOR_EXCEPTION, "Red exception."};


/**
 * Exception hierarchy
 */
TEST_CASE{

    int caught = 0;

    TRY {

        THROW(RED_EXCEPTION, "This is a red exception");

        TEST_FAIL; /* this should not happen */

    } CATCH(COLOR_EXCEPTION) {

        printf("The color exception was caught: %s\n", THROWN_EXCEPTION.name);

        caught = 1;
    }

    TEST_ASSERT(caught);
}
