
# include "testing.h"
# include <signal.h>


E4C_DEFINE_EXCEPTION(CustomException, "This is a custom exception", RuntimeException);
e4c_signal_mapping custom_mappings[3] = {

    E4C_IGNORE_SIGNAL(SIGTERM),
    E4C_SIGNAL_MAPPING(SIGINT, CustomException),
    E4C_NULL_SIGNAL_MAPPING
};


/**
 * Setting custom signal mappings
 *
 * This test sets custom *signal mappings*. Then a couple of *signals* are
 * raised. The first one is ignored; the second one is converted into a
 * user-defined exception.
 *
 */
TEST_CASE{

    volatile bool SIGTERM_was_ignored = false;
    volatile bool SIGINT_was_thrown = false;

    e4c_context_begin(false);

    e4c_context_set_signal_mappings(custom_mappings);

    TEST_ASSERT_EQUALS(e4c_context_get_signal_mappings(), custom_mappings);

    E4C_TRY{

        raise(SIGTERM);

        SIGTERM_was_ignored = true;

        SIGINT_was_thrown = true;

        raise(SIGINT);

        SIGINT_was_thrown = false;

    }E4C_CATCH(RuntimeException){

        TEST_ASSERT_EQUALS(e4c_get_exception()->type, &CustomException);
    }

    e4c_context_end();

    TEST_ASSERT(SIGTERM_was_ignored);
    TEST_ASSERT(SIGINT_was_thrown);
}
