
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <exceptions4c-pthreads.h>

const struct e4c_exception_type MY_EXCEPTION = {NULL, "My exception."};

static void foobar(const char * thread_name);
static void * catching_exception(void * arg);
static void * not_catching_exception(void * arg);

#define THREADS 100

/**
 * Multithreaded unit test
 */
int main(void) {

    int error;
    pthread_t thread[THREADS];
    int result[THREADS];
    bool failed = false;

    e4c_set_context_supplier(e4c_pthreads_context_supplier);

    int index;
    for (index = 0; index < THREADS; index++) {
        /* assume error */
        result[index] = 1;
        error = pthread_create(&thread[index], NULL, index % 2 ? catching_exception : not_catching_exception, &result[index]);
        if (error) {
            errno = error;
            perror("pthread_join");
            fflush(stderr);
        }
    }

    for (index = 0; index < THREADS; index++) {
        error = pthread_join(thread[index], NULL);
        if (error) {
            errno = error;
            perror("pthread_join");
            fflush(stderr);
        }
        if (result[index]) {
            failed = true;
        }
    }

    return failed;
}

static void foobar(const char * thread_name) {

    THROW(MY_EXCEPTION, "Exception thrown by: %s", thread_name);
}


static void * catching_exception(void * arg) {
    int * result = arg;
    const pthread_t self = pthread_self();

    printf("Starting thread #%llu\n", (long long unsigned) self);
    fflush(stdout);

    TRY {

        foobar("CATCHING_EXCEPTION");

    } CATCH(MY_EXCEPTION) {
        const struct e4c_exception * thrown = e4c_get_exception();

        printf("Thread #%llu: Caught %s: %s\n", (long long unsigned) self, thrown->name, thrown->message);
        fflush(stdout);

        /* if the exception is caught and the message matches, then there was no error */
        *result = strcmp(thrown->message, "Exception thrown by: CATCHING_EXCEPTION");
    }

    return NULL;
}

static void * not_catching_exception(void * arg) {
    int * result = arg;
    const pthread_t self = pthread_self();

    printf("Starting thread #%llu\n", (long long unsigned) self);
    fflush(stdout);

    /* assume no error */
    *result = 0;

    TRY {
        foobar("NOT_CATCHING_EXCEPTION");
    } FINALLY {
        const struct e4c_exception * thrown = e4c_get_exception();

            /* if the exception is caught, there was an error */
        if (!e4c_is_uncaught()) {
            *result = 1;
        }

        printf("Thread #%llu: Uncaught %s: %s\n", (long long unsigned) self, thrown->name, thrown->message);
        fflush(stdout);
    }

    /* if this thread is still running, there was an error */
    *result = 1;

    return NULL;
}
