
#include "testing.h"

const struct e4c_exception_type FATAL_ERROR = {NULL, "Fatal error."};

/**
 * Uncaught exception
 */
TEST_CASE{

    THROW(FATAL_ERROR, "This is an uncaught exception");
}
