
# include "testing.h"


/**
 * `e4c_get_exception` call without starting a new exception frame
 *
 * The library must return NULL.
 *
 */
TEST_CASE{

    TEST_ASSERT_EQUALS(e4c_get_exception(), NULL);
}
