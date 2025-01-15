/*
 * exceptions4c lightweight version 2.0
 *
 * Copyright (c) 2016 Guillermo Calvo
 * Licensed under the GNU Lesser General Public License
 */

#include <stdlib.h>
#include <stdio.h>
#include "exceptions4c-lite.h"

const struct e4c_exception_type RuntimeException = {&RuntimeException, "Runtime exception."};
const struct e4c_exception_type NullPointerException = {&RuntimeException, "Null pointer."};

struct e4c_context e4c = {0};
static const char * err_msg[] = {"\n\nError: %s (%s)\n\n", "\n\nUncaught %s: %s\n\n    thrown at %s:%d\n\n"};

static void e4c_propagate(void) {

    e4c.block[e4c.blocks].uncaught = 1;

    if (e4c.blocks > 0) {
        longjmp(e4c.block[e4c.blocks - 1].jump, 1);
    }

    if (fprintf(stderr, e4c.err.file ? err_msg[1] : err_msg[0], e4c.err.name, e4c.err.message, e4c.err.file, e4c.err.line) > 0) {
        (void) fflush(stderr);
    }

    exit(EXIT_FAILURE);
}

int e4c_try(const char * file, int line) {

    if (e4c.blocks >= sizeof(e4c.block) / sizeof(e4c.block[0])) {
        e4c_throw(&RuntimeException, "RuntimeException", file, line, "Too many `try` blocks nested.");
    }

    e4c.blocks++;

    e4c.block[e4c.blocks].stage = e4c_beginning;
    e4c.block[e4c.blocks].uncaught = 0;

    return 1;
}

int e4c_hook(int is_catch) {

    int uncaught;

    if (is_catch) {
        e4c.block[e4c.blocks].uncaught = 0;
        return 1;
    }

    uncaught = e4c.block[e4c.blocks].uncaught;

    e4c.block[e4c.blocks].stage++;
    if (e4c.block[e4c.blocks].stage == e4c_catching && !uncaught) {
        e4c.block[e4c.blocks].stage++;
    }

    if (e4c.block[e4c.blocks].stage < e4c_done) {
        return 1;
    }

    e4c.blocks--;

    if (uncaught) {
        e4c_propagate();
    }

    return 0;
}

int e4c_extends(const struct e4c_exception_type * child, const struct e4c_exception_type * parent) {

    for (; child && child->supertype != child; child = child->supertype) {
        if (child->supertype == parent) {
            return 1;
        }
    }

    return 0;
}

void e4c_throw(const struct e4c_exception_type * exception_type, const char * name, const char * file, int line, const char * message) {

    e4c.err.type = (exception_type ? exception_type : &NullPointerException);
    e4c.err.name = name;
    e4c.err.file = file;
    e4c.err.line = line;

    (void) sprintf(e4c.err.message, "%.*s", (int) sizeof(e4c.err.message) - 1, (message ? message : e4c.err.type->default_message));

    e4c_propagate();
}
