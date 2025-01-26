
#include <exceptions4c-lite.h>

struct e4c_context exceptions4c = {0};
const e4c_exception_type FATAL_ERROR = "Fatal error.";

/**
 * Uncaught exception
 */
int main(void) {

    THROW(FATAL_ERROR, "This is an uncaught exception");

    return 0;
}
