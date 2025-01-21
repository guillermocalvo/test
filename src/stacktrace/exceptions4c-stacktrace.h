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
 * Stacktrace extension for exceptions4c.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * This extension allows **exceptions4c** to print a trace stack
 * regarding an uncaught exception:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * MY_EXCEPTION: He who foos last, foos best.
 *     thrown at thud (foobar.c:9)
 *     from xyzzy (foobar.c:20)
 *     from plugh (foobar.c:25)
 *     from fred (foobar.c:30)
 *     from waldo (foobar.c:35)
 *     from garply (foobar.c:40)
 *     from grault (foobar.c:45)
 *     from corge (foobar.c:50)
 *     from quux (foobar.c:55)
 *     from qux (foobar.c:63)
 *     from baz (foobar.c:68)
 *     from bar (foobar.c:75)
 *     from foo (foobar.c:84)
 *     from main (foobar.c:96)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * You need to configure the exception context with the provided
 * functions:
 *
 * ```.c
 *
 * #include <exceptions4c-stacktrace.h>
 *
 * struct stacktrace_options stacktraces = {0};
 *
 * int main(int argc, char *argv[]){
 *
 *     stacktraces.binary_path = argv[0];
 *     stacktraces.addr2line_path = NULL;
 *     stacktraces.addr2line_options = "--basenames";
 *
 *     struct e4c_context * context = e4c_get_context();
 *     context->uncaught_handler = stacktrace_uncaught_handler;
 *     context->initialize_exception = stacktrace_initialize_exception;
 *     context->finalize_exception = stacktrace_finalize_exception;
 *
 *     TRY {
 *         // ...
 *     } FINALLY {
 *         // ...
 *     }
 * }
 * ```
 *
 * You need to compile your program with GCC (the GNU Compiler Collection).
 *
 * This extension depends on the *GCC Function Instrumentation* and
 * *Generate debugging information* functionality. They can be enabled by
 * using the compiler parameters:
 *
 *   - `-finstrument-functions`
 *   - `-g3`
 *
 * @file        exceptions4c-stacktrace.h
 * @version     1.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#ifndef EXCEPTIONS4C_STACKTRACE_H
#define EXCEPTIONS4C_STACKTRACE_H

# ifndef EXCEPTIONS4C
#   include "../exceptions4c.h"
# endif

/**
 * Configures how stacktraces are captured for this program.
 */
struct stacktrace_options {

    /**
     * The file path to the program binary.
     */
    const char * binary_path;

    /**
     * The file path to the addr2line binary.
     */
    const char * addr2line_path;

    /**
     * Additional options that will be passed to addr2line.
     */
    const char * addr2line_options;
};

/**
 * Define this global variable to configure stacktraces.
 */
extern struct stacktrace_options stacktraces;

/**
 * Prints an error message and backtrace regarding the uncaught exception.
 *
 * @param exception the uncaught exception.
 *
 * This function prints the exception and its backtrace to the standard
 * error output. The stack trace represents the reverse path of execution
 * at the moment the exception was thrown.
 *
 * @note
 * This function must be set as the uncaught handler of the current
 * exception context.
 *
 * @see     #e4c_context.uncaught_handler
 */
void stacktrace_uncaught_handler(const struct e4c_exception * exception) __attribute__ ((no_instrument_function));

/**
 * Initializes the backtrace of an exception that is being created.
 *
 * @param exception the exception to initialize.
 *
 * Captures the call stack at the moment an exception was thrown.
 *
 * @note
 * This function must be set as the exception initializer function of
 * the current exception context.
 *
 * @see     #e4c_context.initialize_exception
 */
void stacktrace_initialize_exception(struct e4c_exception * exception) __attribute__ ((no_instrument_function));

/**
 * Finalizes the backtrace of an exception that is being destroyed.
 *
 * @param exception the exception to initialize
 *
 * Deallocates the buffer that holds the call stack at the moment an
 * exception was thrown.
 *
 * @note
 * This function must be set as the exception finalizer function of the
 * current exception context.
 *
 * @see     #e4c_context.initialize_exception
 */
void stacktrace_finalize_exception(const struct e4c_exception * exception) __attribute__ ((no_instrument_function));

/**
 * @internal
 * @brief Instrument calls for entry to functions.
 *
 * @param callee the address of the start of the current function
 * @param caller its call site
 *
 * @note
 * This function will be called automatically just after function entry.
 */
void __cyg_profile_func_enter(void * callee, void * caller) __attribute__ ((no_instrument_function));

/**
 * @internal
 * @brief Instrument calls for exit to functions.
 *
 * @param callee the address of the start of the current function
 * @param caller its call site
 *
 * @note
 * This function will be called automatically just before function exit.
 */
void __cyg_profile_func_exit(void * callee, void * caller) __attribute__ ((no_instrument_function));

# endif

