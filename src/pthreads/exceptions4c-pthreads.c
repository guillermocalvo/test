/*
 * Copyright 2025 Guillermo Calvo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Implementation of the thread-safe extension for exceptions4c.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * @file        exceptions4c-pthreads.h
 * @version     1.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#include <stdio.h> /* fflush, fprintf, stderr */
#include <errno.h> /* errno */
#include <pthread.h> /* PTHREAD_CANCELED, PTHREAD_ONCE_INIT, pthread_exit, pthread_getspecific, pthread_key_create, pthread_key_delete, pthread_key_t, pthread_once, pthread_once_t, pthread_setspecific */
#include <exceptions4c-pthreads.h>

#define PANIC_ON_ERROR(ERROR_MESSAGE, FUNCTION, ...)                        \
    do {                                                                    \
        int error_code = FUNCTION(__VA_ARGS__);                             \
        panic_if(error_code != 0, #FUNCTION, ERROR_MESSAGE, error_code);    \
    } while (false)


/* Key for the thread-specific exception context */
static pthread_key_t context_key;

/* Once-only initialization of the key */
static pthread_once_t context_key_once = PTHREAD_ONCE_INIT;

/* Register the cleanup function once only */
static pthread_once_t cleanup_once = PTHREAD_ONCE_INIT;

static struct e4c_context * create_context(void);
static void create_context_key(void);
static void delete_context_key(void);
static void cleanup(void);
static void delete_context(void * context);
static void termination_handler(void);
static void print_error(const char * cause, int error_code, const char * error_message);
static void panic_if(bool failure, const char * cause, const char * error_message, int error_code);

/**
 * Prints a fatal message to the standard error stream.
 *
 * @param cause The name of the function that failed.
 * @param error_code The error code returned by the function that failed.
 * @param error_message The message to print to standard error output.
 */
static void print_error(const char * cause, int error_code, const char * error_message) {
    (void) fprintf(stderr, "\n[exceptions4c-pthreads] Thread #%llu: %s\n",
        (long long unsigned) pthread_self(),
        error_message);
    if (error_code) {
        errno = error_code;
        perror(cause);
    }
    (void) fflush(stderr);
}

/**
 * Causes abnormal program termination due to a fatal error.
 *
 * @param failure Whether to terminate the program or not.
 * @param cause The name of the function that failed.
 * @param error_code The error code returned by the function that failed.
 * @param error_message The message to print to standard error output.
 */
static void panic_if(bool failure, const char * cause, const char * error_message, int error_code) {
    if (failure) {
        print_error(cause, error_code, error_message);
        abort();
    }
}

/**
 * Allocates the context key.
 */
static void create_context_key() {
    PANIC_ON_ERROR("Could not create context key.", pthread_key_create, &context_key, delete_context);
}

/**
 * Deallocates the context key.
 */
static void delete_context_key() {
    PANIC_ON_ERROR("Could not delete context key.", pthread_key_delete, context_key);
}

static void cleanup(void) {
    PANIC_ON_ERROR("Could not register cleanup function.", atexit, delete_context_key);
}

/**
 * Terminates the current thread.
 *
 * This termination handler cancels the current thread, rather than
 * terminating the entire program.
 */
static void termination_handler(void) {
    print_error(NULL, 0, "Terminating due to uncaught exceptions.");
    pthread_exit(PTHREAD_CANCELED);
}

/**
 * Allocates and initializes the thread-specific context.
 */
static struct e4c_context * create_context(void) {
    PANIC_ON_ERROR("Could not initialize context key.", pthread_once, &context_key_once, create_context_key);
    PANIC_ON_ERROR("Could not initialize cleanup function.", pthread_once, &cleanup_once, cleanup);
    struct e4c_context * context = calloc(1, sizeof(*context));
    panic_if(context == NULL, "calloc", "Could not allocate exception context.", errno);
    context->termination_handler = termination_handler;
    PANIC_ON_ERROR("Could not save thread-specific context.", pthread_setspecific, context_key, context);
    return context;
}

/**
 * Deallocates the thread-specific context.
 *
 * @param context the context to deallocate.
 */
static void delete_context(void * context) {
    const void * block = ((struct e4c_context *) context)->_innermost_block;
    free(context);
    panic_if(block != NULL, NULL, "Dangling exception block leaked. Some `TRY` block may have been exited improperly (via `goto`, `break`, `continue`, or `return`).", 0);
}

struct e4c_context * e4c_pthreads_context_supplier() {
    struct e4c_context * context = pthread_getspecific(context_key);
    return context ? context : create_context();
}
