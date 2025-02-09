
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <exceptions4c-pthreads.h>

static void * foobar(void * arg);

static int OK = 123;

/**
 * Multithreaded dangling context unit test
 */
int main(void) {

    pthread_t thread;

    e4c_set_context_supplier(e4c_pthreads_context_supplier);

    int error = pthread_create(&thread, NULL, foobar, NULL);
    if (error) {
        errno = error;
        perror("pthread_join");
        fflush(stderr);
        return EXIT_SUCCESS;
    }

    (void) pthread_join(thread, NULL);

    return EXIT_SUCCESS;
}

static void * foobar(void * arg) {
    TRY {
        goto oops;
    }
    oops:
    return &OK;
}
