
#include <stdlib.h>
#include <exceptions4c.h>
#include <exceptions4c-backtrace.h>

const struct e4c_exception_type MY_EXCEPTION = {NULL, "My exception."};

static void foobar(void) {
    THROW(MY_EXCEPTION, "Backtrace me!");
}

static void bar(void) {
    foobar();
}

static void foo(void) {
    bar();
}

/**
 * Uncaught exception with backtrace.
 */
int main(void) {

    struct e4c_context * context = e4c_get_context();

    context->uncaught_handler = e4c_backtrace_uncaught_handler;
    context->initialize_exception = e4c_backtrace_initialize_exception;
    context->finalize_exception = e4c_backtrace_finalize_exception;

    foo();

    return EXIT_SUCCESS;
}
