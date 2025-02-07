
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

    int error;
    pthread_t thread;
    void * result = NULL;

    e4c_set_context_supplier(e4c_pthreads_context_supplier);

    error = pthread_create(&thread, NULL, foobar, NULL);
    if (error) {
        errno = error;
        perror("pthread_join");
        fflush(stderr);
        return EXIT_FAILURE;
    }

    error = pthread_join(thread, &result);
    if (error) {
        errno = error;
        perror("pthread_join");
        fflush(stderr);
        return EXIT_FAILURE;
    }

    if (result != PTHREAD_CANCELED) {
        fprintf(stderr, "Thread was not canceled\n");
        fflush(stderr);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static void * foobar(void * arg) {
    TRY {
        goto oops;
    }
    oops:
    return &OK;
}
