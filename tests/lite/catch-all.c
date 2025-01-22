
#include <string.h>
#include <exceptions4c-lite.h>


struct e4c_context exceptions4c = {0};
const struct e4c_exception_type MY_EXCEPTION = {"My exception."};


/**
 * Caught exception
 */
int main(void) {

    int caught = 0;
    int message_ok = 0;

    TRY {

        THROW(MY_EXCEPTION, NULL);

    } CATCH_ALL {

        caught = 1;

        printf("The exception was caught: %s: %s\n", THROWN_EXCEPTION.name, THROWN_EXCEPTION.message);

        message_ok = strcmp(THROWN_EXCEPTION.message, "My exception.") == 0;
    }

    return !caught || !message_ok;
}
