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
 * Implementation of the stacktrace extension.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * @file        exceptions4c-stacktrace.c
 * @version     1.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 *
 */

#include <stdio.h>
#include "exceptions4c-stacktrace.h"

# ifndef MAX_NESTED_FUNCTION_CALLS
#   define MAX_NESTED_FUNCTION_CALLS 256
# endif

/** @internal Current number of nested function calls registered. */
static int nested_functions = 0;

/** @internal Stores the addresses of the nested function callers. */
static const void * nested_caller[MAX_NESTED_FUNCTION_CALLS];

static void print_exception(const struct e4c_exception * exception, bool is_cause) __attribute__ ((no_instrument_function));

void stacktrace_uncaught_handler(const struct e4c_exception * exception) {
    print_exception(exception, false);
    for (exception = exception->cause; exception; exception = exception->cause) {
        print_exception(exception, true);
    }
    (void) fflush(stderr);
}

void stacktrace_initialize_exception(struct e4c_exception * exception) {
    int nested = nested_functions > MAX_NESTED_FUNCTION_CALLS ? MAX_NESTED_FUNCTION_CALLS : nested_functions;
    exception->data = calloc(nested + 1, sizeof(void *));
    if (exception->data) {
        // Copy current stack trace in reverse order
        int index;
        for (index = 0; index < nested; index++) {
            ((const void * *) exception->data)[index] = nested_caller[nested - index - 1];
        }
    }
}

void stacktrace_finalize_exception(const struct e4c_exception * exception) {
    free(exception->data);
}

void __cyg_profile_func_enter(void * callee __attribute__ ((unused)), void * caller) {
    if (nested_functions < MAX_NESTED_FUNCTION_CALLS) {
        nested_caller[nested_functions] = caller;
    }
    nested_functions++;
}

void __cyg_profile_func_exit(void * callee __attribute__ ((unused)), void * caller __attribute__ ((unused))) {
    if (nested_functions > 0) {
        nested_functions--;
    }
}

/**
 * Prints an error message and backtrace regarding the uncaught exception.
 *
 * @param exception the exception to print.
 * @param is_cause <tt>true</tt> if the exception is the cause to some other exception; <tt>false</tt> otherwise.
 */
static void print_exception(const struct e4c_exception * exception, bool is_cause) {
    // Print exception message
    fprintf(stderr, "%s%s: %s\n", is_cause ? "Caused by: " : "\n", exception->name, exception->message);
    if (exception->file) {
        if (exception->function) {
            (void) fprintf(stderr, "    at %s (%s:%d)\n", exception->function, exception->file, exception->line);
        } else {
            (void) fprintf(stderr, "    at %s:%d\n", exception->file, exception->line);
        }
    }
    // Print stack trace if available
    const void * * addresses = exception->data;
    if (!addresses || !*addresses) {
        return;
    }
    // Prepare addr2line command
    char command[512] = {0};
    int written = snprintf(command, sizeof(command),
        "%s %s --functions --exe \"%s\"",
        stacktraces.addr2line_path ? stacktraces.addr2line_path : "addr2line",
        stacktraces.addr2line_options ? stacktraces.addr2line_options : "",
        stacktraces.binary_path);
    const void * * address;
    for (address = addresses; *address; address++) {
        written += snprintf(&command[written], sizeof(command) - written, " %p", *address);
    }
    (void) snprintf(&command[written], sizeof(command) - written, " 2>&1");
    // Execute addr2line command
    FILE * pipe = popen(command, "r");
    if (pipe) {
        for (address = addresses; *address; address++) {
            char function[256] = {0};
            char fileline[256] = {0};
            int parsed = fscanf(pipe, "%255[^\n\r]%*[\n\r]%255[^\n\r]%*[\n\r]", function, fileline);
            if (parsed != 2) {
                break;
            }
            if (*function == '_') continue;
            if (*fileline && *fileline != '?') {
                if (*function && *function != '?') {
                    (void) fprintf(stderr, "    from %s (%s)\n", function, fileline);
                } else {
                    (void) fprintf(stderr, "   from %s\n", fileline);
                }
            } else {
                (void) fprintf(stderr, "    from %s @ %p\n", stacktraces.binary_path, *address);
            }
        }
        (void) pclose(pipe);
    }
}
