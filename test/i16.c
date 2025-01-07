
# include "testing.h"


/**
 * Obtaining the version number of the library
 *
 * This test calls `e4c_library_version` to retrieve the version number of
 * exceptions4c.
 *
 */
TEST_CASE{

    long library_version;
    long expected_version = EXCEPTIONS4C_VERSION;

    library_version = e4c_library_version();

    TEST_DUMP("%ld", library_version);

    TEST_ASSERT_EQUALS(library_version, expected_version);
}
