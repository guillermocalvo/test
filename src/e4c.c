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
# include <stdnoreturn.h>
# include "e4c.h"

/** Represents the execution stage of the current exception block */
enum block_stage {
    BEGINNING,
    ACQUIRING,
    TRYING,
    DISPOSING,
    CATCHING,
    FINALIZING,
    DONE
};

/** Represents an exception block */
struct e4c_block {
    struct e4c_block * previous;
    enum block_stage stage;
    bool uncaught;
    struct e4c_exception * thrown_exception;
    int retry_attempts;
    int reacquire_attempts;
    e4c_env env;
};

static noreturn void panic(const char * error_message, const char * file, int line, const char * function);
static void default_uncaught_handler(const struct e4c_exception * exception);
static struct e4c_context * get_context(const char * file, int line, const char * function);
static void cleanup(void);
static void throw(const struct e4c_context * context, const struct e4c_exception_type * type, const char * name, int error_number, const char * file, int line, const char * function, const char * format, va_list arguments_list);
static void propagate(const struct e4c_context * context, struct e4c_exception * exception);
static void deallocate_block(const struct e4c_context * context, struct e4c_block * block);
static enum block_stage get_stage(const char * file, int line, const char * function);
static void deallocate_exception(const struct e4c_context * context, struct e4c_exception * exception);
static void print_debug_info(const char * file, int line, const char * function);
static void print_exception(const char * prefix, const struct e4c_exception * exception);
static bool exception_type_extends(const struct e4c_exception * exception, const struct e4c_exception_type * parent);

/** exception context supplier */
static struct e4c_context * (*context_supplier)(void) = NULL;

/** default exception context of the program */
static struct e4c_context default_context = {
    .current_block = NULL,
    .initialize_handler = NULL,
    .finalize_handler = NULL,
    .uncaught_handler = default_uncaught_handler
};

/** flag to determine if the exception system is initialized */
static volatile bool is_initialized = false;


/* LIBRARY
 ================================================================ */

static void cleanup(void) {

    const struct e4c_context * context = e4c_get_context();

    /* check for dangling context */
    /* check if there are too many blocks left (breaking out of a try block) */
    if (context != NULL && context->current_block != NULL) {
        /* deallocate the dangling blocks */
        deallocate_block(context, context->current_block);
        panic("Dangling exception block found. Some `TRY` block may have been exited improperly (via `return`, `break`, or `goto`).", NULL, 0, NULL);
    }
}

static noreturn void panic(const char * error_message, const char * file, const int line, const char * function) {
    fprintf(stderr, "[exceptions4c] %s\n", error_message);
    print_debug_info(file, line, function);
    fflush(stderr);
    abort();
}

/* CONTEXT
 ================================================================ */

void e4c_set_context_supplier(struct e4c_context * (*supplier)(void)) {
    context_supplier = supplier;
}

struct e4c_context * e4c_get_context(void) {
    return context_supplier != NULL ? context_supplier() : &default_context;
}

static struct e4c_context * get_context(const char * file, const int line, const char * function) {
    struct e4c_context * context = e4c_get_context();
    if (context == NULL) {
        panic("Context supplier returned NULL.", file, line, function);
    }
    return context;
}

static void propagate(const struct e4c_context * context, struct e4c_exception * exception) {

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
    if (block->stage == ACQUIRING) {
        /* if we are in the middle of an acquisition, we don't need to dispose the resource */
        block->stage = DISPOSING;
        /* (that actually jumps over the "disposing" stage) */
    }
}

/* BLOCK
 ================================================================ */

e4c_env * e4c_start(const bool should_acquire, const char * file, const int line, const char * function) {

    if (!is_initialized) {
        /* registers the function cleanup to be called when the program exits */
        is_initialized  = (atexit(cleanup) == 0);
        if (!is_initialized) {
            panic("Cannot register cleanup function to be called at program exit.", file, line, function);
        }
    }

    struct e4c_context * context = get_context(file, line, function);

    /* allocate a new block */
    struct e4c_block * new_block = calloc(1, sizeof(*new_block));

    if (new_block == NULL) {
        panic("Not enough memory to create a new exception block", file, line, function);
    }

    /* initialize block data */
    new_block->previous             = context->current_block;
    new_block->stage                = should_acquire ? BEGINNING : ACQUIRING;
    new_block->uncaught             = false;
    new_block->reacquire_attempts   = 0;
    new_block->retry_attempts       = 0;
    new_block->thrown_exception     = NULL;
    /* jmp_buf is an implementation-defined type */

    /* make it the new current block */
    context->current_block = new_block;

    return &new_block->env;
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

static enum block_stage get_stage(const char * file, const int line, const char * function) {

    const struct e4c_context * context = get_context(file, line, function);
    const struct e4c_block * block = context->current_block;

    if (block == NULL) {
        panic("Invalid exception context state.", file, line, function);
    }

    return block->stage;
}

bool e4c_try(const char * file, const int line, const char * function) {
    return get_stage(file, line, function) == TRYING;
}

bool e4c_finally(const char * file, const int line, const char * function) {
    return get_stage(file, line, function) == FINALIZING;
}

bool e4c_acquire(const char * file, const int line, const char * function) {
    return get_stage(file, line, function) == ACQUIRING;
}

bool e4c_dispose(const char * file, const int line, const char * function) {
    return get_stage(file, line, function) == DISPOSING;
}

bool e4c_catch(const struct e4c_exception_type * type, const char * file, const int line, const char * function) {

    const struct e4c_context * context = get_context(file, line, function);
    struct e4c_block * block = context->current_block;

    if (block == NULL) {
        panic("Invalid exception context state.", file, line, function);
    }

    if (block->stage != CATCHING || block->thrown_exception == NULL) {
        return false;
    }

    /* does this block catch current exception? */
    if (type == NULL || exception_type_extends(block->thrown_exception, type)) {

        /* yay, catch current exception by executing the handler */
        block->uncaught = false;

        return true;
    }

    /* nay, keep looking for an exception handler */
    return false;
}

bool e4c_next(const char * file, const int line, const char * function) {

    struct e4c_context *    context;
    struct e4c_block *      block;
    struct e4c_block *      previous;
    struct e4c_exception *  thrown_exception;

    context = get_context(file, line, function);

    if (context->current_block == NULL) {
        panic("Invalid exception context state.", file, line, function);
    }

    block = context->current_block;

    block->stage++;

    /* simple optimization */
    if (block->stage == CATCHING && (!block->uncaught || block->thrown_exception == NULL)) {
        /* if no exception was thrown, we don't need to go through the "catching" stage */
        block->stage++;
    }

    /* keep looping until we reach the "done" stage */
    if (block->stage < DONE) {
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
        propagate(context, thrown_exception);
    }

    /* get out of the exception loop */
    return false;
}

e4c_env * e4c_restart(const bool should_reacquire, const int max_repeat_attempts, const struct e4c_exception_type * type, const char * name, const char * file, const int line, const char * function, const char * format, ...) {

    /* store the current error number up front */
    const int error_number = errno;

    const struct e4c_context * context = get_context(file, line, function);

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
        va_list arguments_list;
        va_start(arguments_list, format);
        throw(context, type, name, error_number, file, line, function, format, arguments_list);
        va_end(arguments_list);
    } else {
        /* deallocate previously thrown exception */
        deallocate_exception(context, block->thrown_exception);
        /* reset exception information */
        block->thrown_exception = NULL;
        block->uncaught         = false;
        block->stage            = should_reacquire ? BEGINNING : ACQUIRING;
    }

    /* jump back to the TRY or WITH block */
    return &block->env;
}

bool e4c_is_uncaught(void) {

    const struct e4c_context * context = e4c_get_context();

    return context != NULL && context->current_block != NULL && ((struct e4c_block *) context->current_block)->uncaught;
}

e4c_env * e4c_get_env(void) {

    const struct e4c_context * context = e4c_get_context();

    return context != NULL && context->current_block != NULL ? &((struct e4c_block *) context->current_block)->env : NULL;
}

/* EXCEPTION TYPE
 ================================================================ */

static bool exception_type_extends(const struct e4c_exception * exception, const struct e4c_exception_type * parent) {

    const struct e4c_exception_type * child;

    for (child = exception->type; child != NULL; child = child != child->supertype ? child->supertype : NULL) {

        if (child == parent) {

            return true;
        }
    }

    return false;
}

/* EXCEPTION
 ================================================================ */

const struct e4c_exception * e4c_get_exception(void) {

    const struct e4c_context * context = e4c_get_context();

    return context != NULL && context->current_block != NULL ? ((struct e4c_block *) context->current_block)->thrown_exception : NULL;
}

static void throw(const struct e4c_context * context, const struct e4c_exception_type * type, const char * name, int error_number, const char * file, const int line, const char * function, const char * format, va_list arguments_list) {

    /* allocate new exception */
    struct e4c_exception * exception = calloc(1, sizeof(*exception));

    /* make sure there was enough memory */
    if (exception == NULL) {
        panic("Not enough memory to create a new exception", file, line, function);
    }

    /* "instantiate" the specified exception */
    exception->_ref_count   = 1;
    exception->name         = name;
    exception->file         = file;
    exception->line         = line;
    exception->function     = function;
    exception->error_number = error_number;
    exception->type         = type;
    exception->cause        = NULL;
    exception->custom_data  = NULL;

    if (format == NULL && type != NULL) {
        (void) snprintf(exception->message, sizeof(exception->message), "%s", type->default_message);
    } else if (format != NULL) {
        (void) vsnprintf(exception->message, sizeof(exception->message), format, arguments_list);
    }

    /* capture the cause of this exception */
    const struct e4c_block * block;
    for (block = context->current_block; block != NULL; block = block->previous) {
        if (block->thrown_exception != NULL) {
            exception->cause = block->thrown_exception;
            block->thrown_exception->_ref_count++;
            break;
        }
    }

    /* initialize custom data */
    if (context->initialize_handler != NULL) {
        exception->custom_data = context->initialize_handler(exception);
    }

    propagate(context, exception);
}

e4c_env * e4c_throw(const struct e4c_exception_type * type, const char * name, const char * file, const int line, const char * function, const char * format, ...) {

    const int error_number = errno;
    const struct e4c_context * context = get_context(file, line, function);

    va_list arguments_list;
    va_start(arguments_list, format);
    throw(context, type, name, error_number, file, line, function, format, arguments_list);
    va_end(arguments_list);

    return &((struct e4c_block *) context->current_block)->env;
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

static void print_debug_info(const char * file, const int line, const char * function) {
    if (file != NULL) {
        if (function != NULL) {
            fprintf(stderr, "    at %s (%s:%d)\n", function, file, line);
        } else {
            fprintf(stderr, "    at %s:%d\n", file, line);
        }
    }
}

static void print_exception(const char * prefix, const struct e4c_exception * exception) {

    fprintf(stderr, "%s%s: %s\n", prefix, exception->name, exception->message);

    print_debug_info(exception->file, exception->line, exception->function);

    if (exception->cause != NULL && exception->cause != exception) {
        print_exception("Caused by: ", exception->cause);
    }
}

static void default_uncaught_handler(const struct e4c_exception * exception) {

    print_exception("\n", exception);
    (void) fflush(stderr);
}
