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
 * Implementation of the backtrace extension for exceptions4c.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * @file        exceptions4c-backtrace.h
 * @version     1.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev);
 *              original code contributed by
 *              Tal Liron <tal.liron@gmail.com>
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#include <stdio.h> /* fflush, fprintf, stderr */
#include <unistd.h> /* STDERR_FILENO */
#include <execinfo.h> /* backtrace, backtrace_symbols_fd */
#include <exceptions4c-backtrace.h>

#define BACKTRACE_MAX_SIZE 64

struct back_trace {
    void * frames[BACKTRACE_MAX_SIZE];
    size_t count;
};

static void print_exception(const struct e4c_exception * exception, const bool is_cause) {
    (void) fprintf(stderr, "%s%s: %s\n", is_cause ? "Caused by: " : "\n", exception->name, exception->message);
    struct back_trace * back_trace = exception->data;
    if (back_trace) {
        backtrace_symbols_fd(back_trace->frames, back_trace->count, STDERR_FILENO);
    }
    if (exception->cause != NULL && exception->cause != exception) {
        print_exception(exception->cause, true);
    }
}

void e4c_backtrace_initialize_exception(struct e4c_exception * exception) {
    exception->data = calloc(1, sizeof(struct back_trace));
    if (exception->data) {
        struct back_trace * back_trace = exception->data;
        back_trace->count = backtrace(back_trace->frames, BACKTRACE_MAX_SIZE);
    }
}

void e4c_backtrace_finalize_exception(const struct e4c_exception * exception) {
    free(exception->data);
}

void e4c_backtrace_uncaught_handler(const struct e4c_exception * exception) {
    print_exception(exception, false);
    fflush(stderr);
}
