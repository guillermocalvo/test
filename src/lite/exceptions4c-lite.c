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
 * Implementation of the really lightweight exception handling library for C.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * @file        exceptions4c-lite.c
 * @version     4.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#include <stdlib.h>
#include <stdio.h>
#include "exceptions4c-lite.h"

/** Main exception context of the program. */
struct e4c_context exceptions4c = {0};

/**
 * @internal
 * @brief Prints a message to the standard error output.
 *
 * @param name the name of the error.
 * @param message a text message describing the specific problem.
 * @param file the name of the source code file that caused the error.
 * @param line the number of line that caused the error.
 */
static void print_error(const char * name, const char * message, const char * file, const int line) {
    (void) fprintf(stderr, "\n%s: %s\n", name, message);
    if (file != NULL) {
        (void) fprintf(stderr, "    at %s:%d\n", file, line);
    }
    (void) fflush(stderr);
}

/**
 * @internal
 * @brief Propagates the supplied exception in the supplied context.
 *
 * @note
 * If the exception reached the top level of the program, then it will be
 * printed and the program will be abruptly terminated.
 */
static void propagate(void) {
    if (exceptions4c.blocks > 0) {
        exceptions4c.block[exceptions4c.blocks - 1].uncaught = 1;
        longjmp(exceptions4c.block[exceptions4c.blocks - 1].jump, 1);
    }
    print_error(THROWN_EXCEPTION.name, THROWN_EXCEPTION.message, THROWN_EXCEPTION.file, THROWN_EXCEPTION.line);
    exit(EXIT_FAILURE);
}

void e4c_start(const char * file, int line) {

    if (exceptions4c.blocks >= EXCEPTIONS4C_MAX_BLOCKS) {
        print_error("[exceptions4c]", "Too many `try` blocks nested.", file, line);
        abort();
    }

    exceptions4c.blocks++;
    exceptions4c.block[exceptions4c.blocks - 1].stage = EXCEPTIONS4C_START;
    exceptions4c.block[exceptions4c.blocks - 1].uncaught = 0;
}

int e4c_catch(const struct e4c_exception_type * type) {
    if (type == NULL) {
        exceptions4c.block[exceptions4c.blocks - 1].uncaught = 0;
        return 1;
    }
    const struct e4c_exception_type * subtype;
    for (subtype = THROWN_EXCEPTION.type; subtype != NULL; subtype = subtype != subtype->supertype ? subtype->supertype : NULL) {
        if (subtype == type) {
            exceptions4c.block[exceptions4c.blocks - 1].uncaught = 0;
            return 1;
        }
    }
    return 0;
}

int e4c_next(void) {

    exceptions4c.block[exceptions4c.blocks - 1].stage++;
    if (exceptions4c.block[exceptions4c.blocks - 1].stage == EXCEPTIONS4C_CATCH && !exceptions4c.block[exceptions4c.blocks - 1].uncaught) {
        /* simple optimization: if no exception was thrown, CATCHING stage can be skipped */
        exceptions4c.block[exceptions4c.blocks - 1].stage++;
    }

    if (exceptions4c.block[exceptions4c.blocks - 1].stage < EXCEPTIONS4C_DONE) {
        return 1;
    }

    exceptions4c.blocks--;

    if (exceptions4c.block[exceptions4c.blocks].uncaught) {
        propagate();
    }

    return 0;
}

void e4c_throw(const struct e4c_exception_type * type, const char * name, const char * file, int line, const char * message) {
    THROWN_EXCEPTION.type = type;
    THROWN_EXCEPTION.name = name;
    THROWN_EXCEPTION.file = file;
    THROWN_EXCEPTION.line = line;
    (void) sprintf(THROWN_EXCEPTION.message, "%.*s", (int) sizeof(THROWN_EXCEPTION.message) - 1, (message ? message : type ? type->default_message : ""));
    propagate();
}
