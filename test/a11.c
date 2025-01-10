
# include "testing.h"


/**
 * `e4c_get_exception` call without beginning
 *
 * This test uses the library improperly, by attempting to call
 * `e4c_get_exception`, without calling `e4c_context_begin` first.
 *
 * The library must return NULL.
 *
 */
TEST_CASE{

    TEST_ASSERT_EQUALS(e4c_get_exception(), NULL);
}
