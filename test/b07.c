
# include "testing.h"


/**
 * `e4c_context_begin` call after having already begun
 *
 * This test uses the library in an inconsistent way, by attempting to call
 * `e4c_context_begin` twice in a row.
 *
 */
TEST_CASE{

    e4c_context_begin();

    e4c_context_begin();
}
