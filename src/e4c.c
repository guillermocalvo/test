/*
 *
 * @file        e4c.c
 *
 * exceptions4c source code file
 *
 * @version     4.0
 * @author      Copyright (c) 2016 Guillermo Calvo
 *
 * This is free software: you can redistribute it and/or modify it under the
 * terms of the **GNU Lesser General Public License** as published by the
 * *Free Software Foundation*, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * **WITHOUT ANY WARRANTY**; without even the implied warranty of
 * **MERCHANTABILITY** or **FITNESS FOR A PARTICULAR PURPOSE**. See the
 * [GNU Lesser General Public License](http://www.gnu.org/licenses/lgpl.html)
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * e4c.c is undocumented on purpose (everything is documented in e4c.h)
 */


# include <stdio.h>
# include <errno.h>
# include <stdarg.h>
# include <assert.h>
# include "e4c.h"

/** Represents an exception block */
struct e4c_block {
    struct e4c_block *          previous;
    enum e4c_block_stage        stage;
    bool                        uncaught;
    struct e4c_exception *      thrown_exception;
    int                         retry_attempts;
    int                         reacquire_attempts;
    e4c_jump_buffer             continuation;
};

static struct e4c_context * (*context_supplier)(void) = NULL;

/** flag to determine if the exception system is initialized */
static volatile bool is_initialized = false;

/** main exception context of the program */
static struct e4c_context main_context = {
    .current_block = NULL,
    .custom_data = NULL,
    .initialize_handler = NULL,
    .finalize_handler = NULL,
    .uncaught_handler = e4c_print_exception
};

const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
const struct e4c_exception_type NullPointerException = {&RuntimeException, "Null pointer."};

static void cleanup(void);
static noreturn void panic(const char * error_message, const char * file, int line, const char * function);
static noreturn void propagate_exception(const struct e4c_context * context, struct e4c_exception * exception);
static void deallocate_block(const struct e4c_context * context, struct e4c_block * block);
static void deallocate_exception(const struct e4c_context * context, struct e4c_exception * exception);
static void print_debug_info(const char * file, int line, const char * function);
static void print_exception(const char * prefix, const struct e4c_exception * exception);
static bool exception_type_extends(const struct e4c_exception_type * child, const struct e4c_exception_type * parent);




/* LIBRARY
 ================================================================ */

static void cleanup(void) {

    struct e4c_context * context = e4c_get_current_context();

    /* check for dangling context */
    /* check if there are too many blocks left (breaking out of a try block) */
    if (context->current_block != NULL) {
        /* deallocate the dangling blocks */
        deallocate_block(context, context->current_block);
        panic("Dangling exception block found. Some `TRY` block may have been exited improperly (via `return`, `break`, or `goto`).", E4C_DEBUG_INFO);
    }
}

static noreturn void panic(const char * error_message, const char * file, int line, const char * function) {
    fprintf(stderr, "[exceptions4c] %s\n", error_message);
    print_debug_info(file, line, function);
    fflush(stderr);
    abort();
}

int e4c_library_version(void) {

    return EXCEPTIONS4C_VERSION;
}

/* CONTEXT
 ================================================================ */

void e4c_set_context_supplier(struct e4c_context * (*supplier)(void)) {
    context_supplier = supplier;
}

struct e4c_context * e4c_get_current_context(void) {
    if (context_supplier ==  NULL) {
        return &main_context;
    }
    struct e4c_context * context = context_supplier();
    if (context == NULL) {
        panic("Context supplier returned NULL.", E4C_DEBUG_INFO);
    }
    return context;
}

static void propagate_exception(const struct e4c_context * context, struct e4c_exception * exception) {

    assert(context != NULL);
    assert(exception != NULL);

    struct e4c_block * block = context->current_block;

    /* if this is the outermost block, then this is an uncaught exception */
    if (block == NULL) {
        if (context->uncaught_handler != NULL) {
            context->uncaught_handler(exception);
        }
        exit(EXIT_FAILURE);
    }

    /* update the block with the exception information */
    block->uncaught = true;

    /* deallocate previously thrown exception */
    deallocate_exception(context, block->thrown_exception);

    /* update current thrown exception */
    block->thrown_exception = exception;

    /* otherwise, we will jump to the outer block */

    /* simple optimization */
    if (block->stage == e4c_acquiring) {
        /* if we are in the middle of an acquisition, we don't need to dispose the resource */
        block->stage = e4c_disposing;
        /* (that actually jumps over the "disposing" stage) */
    }

    /* keep looping */
    EXCEPTIONS4C_LONG_JUMP(block->continuation);
}

/* BLOCK
 ================================================================ */

e4c_jump_buffer * e4c_start(bool should_acquire, const char * file, int line, const char * function) {

    if (!is_initialized) {
        /* registers the function cleanup to be called when the program exits */
        is_initialized  = (atexit(cleanup) == 0);
        if (!is_initialized) {
            panic("Cannot register cleanup function to be called at program exit.", E4C_DEBUG_INFO);
        }
    }

    struct e4c_context * context = e4c_get_current_context();

    /* allocate a new block */
    struct e4c_block * new_block = calloc(1, sizeof(*new_block));

    if (new_block == NULL) {
        panic("Not enough memory to create a new exception block", file, line, function);
    }

    /* initialize block data */
    assert(new_block->previous == NULL);
    new_block->previous             = context->current_block;
    new_block->stage                = should_acquire ? e4c_beginning : e4c_acquiring;
    new_block->uncaught             = false;
    new_block->reacquire_attempts   = 0;
    new_block->retry_attempts       = 0;
    new_block->thrown_exception     = NULL;
    /* jmp_buf is an implementation-defined type */

    /* make it the new current block */
    context->current_block = new_block;

    return &new_block->continuation;
}

static void deallocate_block(const struct e4c_context * context, struct e4c_block * block) {

    if (block != NULL) {

        /* delete previous block */
        deallocate_block(context, block->previous);
        block->previous = NULL;

        /* delete thrown exception */
        deallocate_exception(context, block->thrown_exception);
        block->thrown_exception = NULL;

        free(block);
    }
}

enum e4c_block_stage e4c_get_current_stage(void) {

    struct e4c_context * context = e4c_get_current_context();

    assert(context->current_block != NULL);

    return context->current_block->stage;
}

bool e4c_catch(const struct e4c_exception_type * exception_type) {

    struct e4c_context * context = e4c_get_current_context();

    /* passing NULL to a catch block will not catch any exception */
    if (exception_type == NULL) {
        return false;
    }

    assert(context->current_block != NULL);

    if (context->current_block->stage != e4c_catching) {
        return false;
    }

    assert(context->current_block->thrown_exception != NULL);
    assert(context->current_block->thrown_exception->type != NULL);

    /* thrown exception is catchable (otherwise we would have skipped the "catching" stage in e4c_next_stage) */

    /* does this block catch current exception? */
    if (context->current_block->thrown_exception->type == exception_type || exception_type_extends(context->current_block->thrown_exception->type, exception_type)) {

        /* yay, catch current exception by executing the handler */
        context->current_block->uncaught = false;

        return true;
    }

    /* nay, keep looking for an exception handler */
    return false;
}

bool e4c_next_stage(void) {

    struct e4c_context *    context;
    struct e4c_block *      block;
    struct e4c_block *      previous;
    struct e4c_exception *  thrown_exception;

    context = e4c_get_current_context();

    /* make sure the current block is not null */
    assert(context->current_block != NULL);

    block = context->current_block;

    block->stage++;

    /* simple optimization */
    if (block->stage == e4c_catching && (!block->uncaught || block->thrown_exception == NULL)) {
        /* if no exception was thrown, we don't need to go through the "catching" stage */
        block->stage++;
    }

    /* keep looping until we reach the "done" stage */
    if (block->stage < e4c_done) {
        return true;
    }

    /* the exception loop is finished */

    /* deallocate caught exception */
    if (block->thrown_exception != NULL && !block->uncaught) {
        deallocate_exception(context, block->thrown_exception);
        block->thrown_exception = NULL;
    }

    /* capture temporarily the information of the current block */
    /* so we can propagate an exception (if it was thrown) */
    previous            = block->previous;
    thrown_exception    = block->thrown_exception;

    /* modify the current block so that previous and thrown_exception don't get deallocated */
    block->previous         = NULL;
    block->thrown_exception = NULL;

    /* delete the current block */
    deallocate_block(context, block);

    /* promote the previous block to the current one */
    context->current_block = previous;

    /* if the current block has an uncaught exception, then we will propagate it */
    if (thrown_exception != NULL) {
        propagate_exception(context, thrown_exception);
    }
    /* otherwise, we're free to go */

    /* get out of the exception loop */
    return false;
}

noreturn void e4c_restart(const bool should_reacquire, const int max_repeat_attempts, const struct e4c_exception_type * exception_type, const char * name, const char * file, const int line, const char * function, const char * format, ...) {

    struct e4c_context * context = e4c_get_current_context();

    /* get the current block */
    struct e4c_block * block = context->current_block;

    /* check if 'e4c_restart' was used before 'try' or 'use' */
    if (block == NULL) {
        if (should_reacquire) {
            panic("No `WITH` block to reacquire.", file, line, function);
        }
        panic("No `TRY` block to retry.", file, line, function);
    }

    /* check if maximum number of attempts reached and update the number of attempts */
    bool max_reached;

    if (should_reacquire) {
        /* reacquire */
        max_reached = block->reacquire_attempts >= max_repeat_attempts;
        if (!max_reached) {
            block->reacquire_attempts++;
        }
    } else {
        /* retry */
        max_reached = block->retry_attempts >= max_repeat_attempts;
        if (!max_reached) {
            block->retry_attempts++;
        }
    }

    if (max_reached) {
        if (format == NULL) {
            e4c_throw(exception_type, name, file, line, function, NULL);
        }
        e4c_exception_message message = {0};
        va_list arguments_list;
        va_start(arguments_list, format);
        (void) vsnprintf(message, sizeof(message), format, arguments_list);
        va_end(arguments_list);
        e4c_throw(exception_type, name, file, line, function, message);
    }

    /* deallocate previously thrown exception */
    deallocate_exception(context, block->thrown_exception);

    /* reset exception information */
    block->thrown_exception = NULL;
    block->uncaught         = false;
    block->stage            = should_reacquire ? e4c_beginning : e4c_acquiring;

    /* keep looping */
    EXCEPTIONS4C_LONG_JUMP(block->continuation);
}

enum e4c_status e4c_get_status(void) {

    struct e4c_context * context = e4c_get_current_context();

    assert(context->current_block != NULL);

    if (context->current_block->thrown_exception == NULL) {
        return e4c_succeeded;
    }

    if (context->current_block->uncaught) {
        return e4c_failed;
    }

    return e4c_recovered;
}

/* EXCEPTION TYPE
 ================================================================ */

static bool exception_type_extends(const struct e4c_exception_type * child, const struct e4c_exception_type * parent) {

    assert(child != parent);
    assert(child != NULL);
    assert(parent != NULL);

    for (; child->supertype != NULL && child->supertype != child; child = child->supertype) {

        if (child->supertype == parent) {

            return true;
        }
    }

    return false;
}

bool e4c_is_instance_of(const struct e4c_exception * instance, const struct e4c_exception_type * exception_type) {

    if (instance == NULL || instance->type == NULL || exception_type == NULL) {
        return false;
    }

    if (instance->type == exception_type) {
        return true;
    }

    return exception_type_extends(instance->type, exception_type);
}

/* EXCEPTION
 ================================================================ */

const struct e4c_exception * e4c_get_exception(void) {

    struct e4c_context * context = e4c_get_current_context();

    return context->current_block != NULL ? context->current_block->thrown_exception : NULL;
}

void e4c_throw(const struct e4c_exception_type * exception_type, const char * name, const char * file, int line, const char * function, const char * format, ...) {

    int                     error_number;
    struct e4c_context *    context;
    struct e4c_block *      block;
    struct e4c_exception *  new_exception;

    /* store the current error number up front */
    error_number = errno;

    /* convert NULL exception type to NPE */
    if (exception_type == NULL) {
        exception_type = &NullPointerException;
    }

    /* get the current context */
    context = e4c_get_current_context();

    /* allocate new exception */
    new_exception = calloc(1, sizeof(*new_exception));

    /* make sure there was enough memory */
    if (new_exception == NULL) {
        panic("Not enough memory to create a new exception", E4C_DEBUG_INFO);
    }

    /* "instantiate" the specified exception */
    new_exception->_ref_count   = 1;
    new_exception->name         = name;
    new_exception->file         = file;
    new_exception->line         = line;
    new_exception->function     = function;
    new_exception->error_number = error_number;
    new_exception->type         = exception_type;
    new_exception->cause        = NULL;

    if (format == NULL) {
        (void) snprintf(new_exception->message, sizeof(new_exception->message), "%s", exception_type->default_message);
    } else {
        /* format the message */
        va_list arguments_list;
        va_start(arguments_list, format);
        (void) vsnprintf(new_exception->message, sizeof(new_exception->message), format, arguments_list);
        va_end(arguments_list);
    }

    /* get the current block */
    block = context->current_block;

    /* capture the cause of this exception */
    while (block != NULL) {
        if (block->thrown_exception != NULL) {
            new_exception->cause = block->thrown_exception;
            block->thrown_exception->_ref_count++;
            break;
        }
        block = block->previous;
    }

    /* set initial value for custom data */
    new_exception->custom_data = context->custom_data;
    /* initialize custom data */
    if (context->initialize_handler != NULL) {
        new_exception->custom_data = context->initialize_handler(new_exception);
    }

    /* propagate the exception up the call stack */
    propagate_exception(context, new_exception);
}

static void deallocate_exception(const struct e4c_context * context, struct e4c_exception * exception) {

    if (exception != NULL) {

        exception->_ref_count--;

        if (exception->_ref_count <= 0) {

            deallocate_exception(context, exception->cause);

            if (context->finalize_handler != NULL) {
                context->finalize_handler(exception->custom_data);
            }

            free(exception);
        }
    }
}

static void print_debug_info(const char * file, int line, const char * function) {
    if (file != NULL) {
        if (function != NULL) {
            fprintf(stderr, "    at %s (%s:%d)\n", function, file, line);
        } else {
            fprintf(stderr, "    at %s:%d\n", file, line);
        }
    }
}

static void print_exception(const char * prefix, const struct e4c_exception * exception) {

    assert(exception != NULL);

    fprintf(stderr, "%s%s: %s\n", prefix, exception->name, exception->message);

    print_debug_info(exception->file, exception->line, exception->function);

    if (exception->cause != NULL) {
        print_exception("Caused by: ", exception->cause);
    }
}

void e4c_print_exception(const struct e4c_exception * exception) {

    if (exception == NULL) {
        fprintf(stderr, "No exception\n");
    } else {
        print_exception("\n", exception);
    }

    (void) fflush(stderr);
}
