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

static e4c_context_supplier context_supplier = NULL;

/** flag to determine if the exception system is initialized */
static volatile bool is_initialized = false;

/** main exception context of the program */
static struct e4c_context main_context = {
    NULL,
    e4c_print_exception,
    NULL,
    NULL,
    NULL
};

const struct e4c_exception_type RuntimeException = {NULL, "Runtime exception."};
const struct e4c_exception_type NullPointerException = {&RuntimeException, "Null pointer."};

static void cleanup(void);
static noreturn void panic(const char * error_message, const char * file, int line, const char * function);

static struct e4c_context * get_current_context(void);
static noreturn void propagate_exception(const struct e4c_context * context, struct e4c_exception * exception);

static void deallocate_frame(struct e4c_frame * frame, e4c_finalize_handler finalize_handler);
static bool exception_type_extends(const struct e4c_exception_type * child, const struct e4c_exception_type * parent);

static void deallocate_exception(struct e4c_exception * exception, e4c_finalize_handler finalize_handler);
static void print_debug_info(const char * file, int line, const char * function);
static void print_exception(const char * prefix, const struct e4c_exception * exception);




/* LIBRARY
 ================================================================ */

static void cleanup(void) {

    struct e4c_context * context = get_current_context();

    /* check for dangling context */
    /* check if there are too many frames left (breaking out of a try block) */
    if (context->current_frame != NULL) {
        /* deallocate the dangling frames */
        deallocate_frame(context->current_frame, context->finalize_handler);
        panic("Dangling exception frame found. Some `TRY` block may have been exited improperly (via `return`, `break`, or `goto`).", E4C_DEBUG_INFO);
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

void e4c_set_context_supplier(e4c_context_supplier supplier) {
    context_supplier = supplier;
}

static struct e4c_context * get_current_context(void) {
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

    struct e4c_frame * frame = context->current_frame;

    /* if this is the upper frame, then this is an uncaught exception */
    if (frame == NULL) {
        if (context->uncaught_handler != NULL) {
            context->uncaught_handler(exception);
        }
        exit(EXIT_FAILURE);
    }

    /* update the frame with the exception information */
    frame->uncaught = true;

    /* deallocate previously thrown exception */
    deallocate_exception(frame->thrown_exception, context->finalize_handler);

    /* update current thrown exception */
    frame->thrown_exception = exception;

    /* otherwise, we will jump to the upper frame */

    /* simple optimization */
    if (frame->stage == e4c_acquiring) {
        /* if we are in the middle of an acquisition, we don't need to dispose the resource */
        frame->stage = e4c_disposing;
        /* (that actually jumps over the "disposing" stage) */
    }

    /* keep looping */
    EXCEPTIONS4C_LONG_JUMP(frame->continuation);
}

void e4c_context_set_handlers(e4c_uncaught_handler uncaught_handler, void * custom_data, e4c_initialize_handler initialize_handler, e4c_finalize_handler finalize_handler) {

    struct e4c_context * context = get_current_context();

    context->uncaught_handler   = uncaught_handler;
    context->custom_data        = custom_data;
    context->initialize_handler = initialize_handler;
    context->finalize_handler   = finalize_handler;
}

/* FRAME
 ================================================================ */

e4c_jump_buffer * e4c_start(enum e4c_frame_stage stage, const char * file, int line, const char * function) {

    if (!is_initialized) {
        /* registers the function cleanup to be called when the program exits */
        is_initialized  = (atexit(cleanup) == 0);
        if (!is_initialized) {
            panic("Cannot register cleanup function to be called at program exit.", E4C_DEBUG_INFO);
        }
    }

    struct e4c_context * context = get_current_context();

    /* allocate a new frame */
    struct e4c_frame * new_frame = calloc(1, sizeof(*new_frame));

    if (new_frame == NULL) {
        panic("Not enough memory to create a new exception frame", file, line, function);
    }

    /* initialize frame data */
    assert(new_frame->previous == NULL);
    new_frame->previous             = context->current_frame;
    new_frame->stage                = stage;
    new_frame->uncaught             = false;
    new_frame->reacquire_attempts   = 0;
    new_frame->retry_attempts       = 0;
    new_frame->thrown_exception     = NULL;
    /* jmp_buf is an implementation-defined type */

    /* make it the new current frame */
    context->current_frame = new_frame;

    return &new_frame->continuation;
}

static void deallocate_frame(struct e4c_frame * frame, e4c_finalize_handler finalize_handler) {

    if (frame != NULL) {

        /* delete previous frame */
        deallocate_frame(frame->previous, finalize_handler);
        frame->previous = NULL;

        /* delete thrown exception */
        deallocate_exception(frame->thrown_exception, finalize_handler);
        frame->thrown_exception = NULL;

        free(frame);
    }
}

enum e4c_frame_stage e4c_get_current_stage(void) {

    struct e4c_context * context = get_current_context();

    assert(context->current_frame != NULL);

    return context->current_frame->stage;
}

bool e4c_catch(const struct e4c_exception_type * exception_type) {

    struct e4c_context * context = get_current_context();

    /* passing NULL to a catch block will not catch any exception */
    if (exception_type == NULL) {
        return false;
    }

    assert(context->current_frame != NULL);

    if (context->current_frame->stage != e4c_catching) {
        return false;
    }

    assert(context->current_frame->thrown_exception != NULL);
    assert(context->current_frame->thrown_exception->type != NULL);

    /* thrown exception is catchable (otherwise we would have skipped the "catching" stage in e4c_next_stage) */

    /* does this block catch current exception? */
    if (context->current_frame->thrown_exception->type == exception_type || exception_type_extends(context->current_frame->thrown_exception->type, exception_type)) {

        /* yay, catch current exception by executing the handler */
        context->current_frame->uncaught = false;

        return true;
    }

    /* nay, keep looking for an exception handler */
    return false;
}

bool e4c_next_stage(void) {

    struct e4c_context *    context;
    struct e4c_frame *      frame;
    struct e4c_frame *      previous;
    struct e4c_exception *  thrown_exception;

    context = get_current_context();

    /* make sure the current frame is not null */
    assert(context->current_frame != NULL);

    frame = context->current_frame;

    frame->stage++;

    /* simple optimization */
    if (frame->stage == e4c_catching && (!frame->uncaught || frame->thrown_exception == NULL)) {
        /* if no exception was thrown, we don't need to go through the "catching" stage */
        frame->stage++;
    }

    /* keep looping until we reach the "done" stage */
    if (frame->stage < e4c_done) {
        return true;
    }

    /* the exception loop is finished */

    /* deallocate caught exception */
    if (frame->thrown_exception != NULL && !frame->uncaught) {
        deallocate_exception(frame->thrown_exception, context->finalize_handler);
        frame->thrown_exception = NULL;
    }

    /* capture temporarily the information of the current frame */
    /* so we can propagate an exception (if it was thrown) */
    previous            = frame->previous;
    thrown_exception    = frame->thrown_exception;

    /* modify the current frame so that previous and thrown_exception don't get deallocated */
    frame->previous         = NULL;
    frame->thrown_exception = NULL;

    /* delete the current frame */
    deallocate_frame(frame, context->finalize_handler);

    /* promote the previous frame to the current one */
    context->current_frame = previous;

    /* if the current frame has an uncaught exception, then we will propagate it */
    if (thrown_exception != NULL) {
        propagate_exception(context, thrown_exception);
    }
    /* otherwise, we're free to go */

    /* get out of the exception loop */
    return false;
}

noreturn void e4c_restart(const bool should_reacquire, const int max_repeat_attempts, const struct e4c_exception_type * exception_type, const char * name, const char * file, const int line, const char * function, const char * format, ...) {

    struct e4c_context * context = get_current_context();

    /* get the current frame */
    struct e4c_frame * frame = context->current_frame;

    /* check if 'e4c_restart' was used before 'try' or 'use' */
    if (frame == NULL) {
        if (should_reacquire) {
            panic("No `WITH` block to reacquire.", file, line, function);
        }
        panic("No `TRY` block to retry.", file, line, function);
    }

    /* check if maximum number of attempts reached and update the number of attempts */
    bool max_reached;

    if (should_reacquire) {
        /* reacquire */
        max_reached = frame->reacquire_attempts >= max_repeat_attempts;
        if (!max_reached) {
            frame->reacquire_attempts++;
        }
    } else {
        /* retry */
        max_reached = frame->retry_attempts >= max_repeat_attempts;
        if (!max_reached) {
            frame->retry_attempts++;
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
    deallocate_exception(frame->thrown_exception, context->finalize_handler);

    /* reset exception information */
    frame->thrown_exception = NULL;
    frame->uncaught         = false;
    frame->stage            = should_reacquire ? e4c_beginning : e4c_acquiring;

    /* keep looping */
    EXCEPTIONS4C_LONG_JUMP(frame->continuation);
}

enum e4c_status e4c_get_status(void) {

    struct e4c_context * context = get_current_context();

    assert(context->current_frame != NULL);

    if (context->current_frame->thrown_exception == NULL) {
        return e4c_succeeded;
    }

    if (context->current_frame->uncaught) {
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

    struct e4c_context * context = get_current_context();

    return context->current_frame != NULL ? context->current_frame->thrown_exception : NULL;
}

void e4c_throw(const struct e4c_exception_type * exception_type, const char * name, const char * file, int line, const char * function, const char * format, ...) {

    int                     error_number;
    struct e4c_context *    context;
    struct e4c_frame *      frame;
    struct e4c_exception *  new_exception;

    /* store the current error number up front */
    error_number = errno;

    /* convert NULL exception type to NPE */
    if (exception_type == NULL) {
        exception_type = &NullPointerException;
    }

    /* get the current context */
    context = get_current_context();

    /* check context and frame; initialize exception and cause */
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

    /* get the current frame */
    frame = context->current_frame;

    /* capture the cause of this exception */
    while (frame != NULL) {
        if (frame->thrown_exception != NULL) {
            new_exception->cause = frame->thrown_exception;
            frame->thrown_exception->_ref_count++;
            break;
        }
        frame = frame->previous;
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

static void deallocate_exception(struct e4c_exception * exception, e4c_finalize_handler finalize_handler) {

    if (exception != NULL) {

        exception->_ref_count--;

        if (exception->_ref_count <= 0) {

            deallocate_exception(exception->cause, finalize_handler);

            if (finalize_handler != NULL) {
                finalize_handler(exception->custom_data);
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
