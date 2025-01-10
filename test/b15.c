
# include "testing.h"


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

    E4C_TRY {

        E4C_THROW(RuntimeException, NULL);

    } E4C_CATCH(  *( (e4c_exception_type *)NULL )  ) {

        THIS_SHOULD_NOT_HAPPEN;
    }
}
