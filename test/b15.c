
# include "testing.h"

static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * `catch(NULL)`
 *
 * This test uses the library in an inconsistent way, by attempting to pass
 * `NULL` to a `catch` block.
 *
 * The library must ignore the catch block.
 *
 */
TEST_CASE{

    TEST_EXPECTING(RuntimeException);

    TRY {

        THROW(RuntimeException, NULL);

    } CATCH(*((struct e4c_exception_type *) NULL)) {

        THIS_SHOULD_NOT_HAPPEN;
    }
}
