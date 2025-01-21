
# include <stdio.h>
# include <exceptions4c.h>
# include <exceptions4c-stacktrace.h>

const struct e4c_exception_type MY_EXCEPTION = {NULL, "My exception."};

struct stacktrace_options stacktraces = {0};

static void foo() {
    THROW(MY_EXCEPTION, "Oh no!");
}

static void bar() {
    foo();
}

static void foobar() {
    bar();
}

int main(int argc, char *argv[]){

    stacktraces.binary_path = argv[0];
    stacktraces.addr2line_path = NULL;
    stacktraces.addr2line_options = "--basenames";

    struct e4c_context * context = e4c_get_context();

    context->uncaught_handler = stacktrace_uncaught_handler;
    context->initialize_exception = stacktrace_initialize_exception;
    context->finalize_exception = stacktrace_finalize_exception;

    TRY {

        foobar();

    } CATCH_ALL {

        THROW(MY_EXCEPTION, "Oops... I did it again!");

    } FINALLY {

        const struct e4c_exception * exception = e4c_get_exception();

        printf("Finally! %s: %s\n\n", exception->name, exception->message);
        fflush(stdout);
    }
}
