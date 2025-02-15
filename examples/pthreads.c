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

//! [setup]
#include <stdlib.h>
#include <pthread.h>
#include <exceptions4c-pthreads.h>

const struct e4c_exception_type MY_EXCEPTION = {NULL, "My exception"};

static void * my_thread(void * _) {
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
