
# include "testing.h"


/**
 * `use` block right next to a `try` block
 *
 * This test uses the library in an inconsistent way, by attempting to place a
 * `use` block right next to a `try` block.
 *
 * This is plain wrong, because a `use` block must be preceded by a `with`
 * block, so the library must ignore the `use` block.
 *
 */
TEST_CASE{

    volatile bool ignored = false;

    TRY {

        TEST_ECHO("Inside `try` block...");

        ignored = true;

    /* } */ USE {

        TEST_ECHO("Inside `use` block...");

        ignored = false;
    }

    /**
     * In order to be able to compile a 'use' block preceded by a 'try', we
     * have to remove the ending curly brace, otherwise we get a compiler error.
     *
     * Anyway, this test proves that the 'use' block will be ignored.
     */

    TEST_ASSERT(ignored);
}
