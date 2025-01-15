
# include "testing.h"


char foobar[64] = "FOOBAR";
void custom_initializer(struct e4c_exception * exception);
void custom_finalizer(const struct e4c_exception * exception);
volatile bool custom_handler_was_initialized = false;
volatile bool custom_handler_was_finalized = false;
static const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};

/**
 * Setting a custom finalization handler
 *
 * This test sets a custom *finalization handler*. Then *throws* an exception
 * and *catches* it.
 *
 */
TEST_CASE{

    struct e4c_context * context = e4c_get_context();
    context->initialize_exception = custom_initializer;
    context->finalize_exception = custom_finalizer;

    TRY {

        THROW(RuntimeException, "Finalize my custom data");

    } CATCH(RuntimeException) {

        custom_handler_was_initialized = ( strcmp(e4c_get_exception()->data, foobar) == 0 );
    }

    TEST_ASSERT(custom_handler_was_initialized);
    TEST_ASSERT(custom_handler_was_finalized);
}

void custom_initializer(struct e4c_exception * exception) {
    exception->data = foobar;
}

void custom_finalizer(const struct e4c_exception * exception) {

    TEST_ASSERT_EQUALS(exception->data, foobar);

    custom_handler_was_finalized = true;
}
