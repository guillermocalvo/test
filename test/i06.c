
# include "testing.h"


typedef struct{ int id; const char * name; } custom_data_t;
custom_data_t initial_data = {123, "FOOBAR"};
static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Defining custom data
 *
 * This test defines the custom data that will be used every time a new
 * exception is thrown. Then *throws* an exception; the `catch` block inspects
 * the exception's *custom data*.
 *
 */
TEST_CASE{

    e4c_get_context()->custom_data = &initial_data;

    TRY {

        THROW(RuntimeException, "Initialize my custom data with a default value");

    } CATCH(RuntimeException) {

        custom_data_t * data = e4c_get_exception()->custom_data;

        TEST_ASSERT_EQUALS(data->id, initial_data.id);
        TEST_ASSERT_STRING_EQUALS(data->name, initial_data.name);
    }
}
