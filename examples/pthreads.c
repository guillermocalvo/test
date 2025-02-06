//! [setup]
#include <stdlib.h>
#include <pthread.h>
#include <exceptions4c-pthreads.h>

const struct e4c_exception_type MY_EXCEPTION = {NULL, "My exception"};

static void * my_thread(void * arg) {
    THROW(MY_EXCEPTION, "Oops!");
}

int main(void) {
    pthread_t thread;

    e4c_set_context_supplier(e4c_pthreads_context_supplier);

    pthread_create(&thread, NULL, my_thread, NULL);
    pthread_join(thread, NULL);

    return EXIT_SUCCESS;
}
//! [setup]

static void cancel_current_thread() {
    pthread_exit(PTHREAD_CANCELED);
}

struct e4c_context dummy = {
    ._innermost_block = NULL,
    .uncaught_handler = NULL,
    .termination_handler = cancel_current_thread,
    .initialize_exception = NULL,
    .finalize_exception = NULL
};

struct e4c_context * e4c_pthreads_context_supplier() {
    return &dummy;
}
