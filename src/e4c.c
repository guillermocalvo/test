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

# define INITIALIZE_ONCE                if(!is_initialized){ library_initialize(); }

# define ref_count                      _

# define VERBATIM_COPY(dst, src) (void)snprintf(dst, (size_t)E4C_EXCEPTION_MESSAGE_SIZE, "%s", src)

#define E4C_CURRENT_CONTEXT      current_context

/* unless otherwise stated, SIGSTOP and SIGKILL cannot be caught or ignored */

typedef struct e4c_frame_ e4c_frame;
struct e4c_frame_ {
    e4c_frame *                 previous;
    enum e4c_frame_stage        stage;
    bool                        uncaught;
    e4c_exception *             thrown_exception;
    int                         retry_attempts;
    int                         reacquire_attempts;
    e4c_jump_buffer             continuation;
};

typedef struct e4c_context_ e4c_context;
struct e4c_context_ {
    e4c_frame *                 current_frame;
    e4c_uncaught_handler        uncaught_handler;
    void *                      custom_data;
    e4c_initialize_handler      initialize_handler;
    e4c_finalize_handler        finalize_handler;
};

/** flag to signal a critical error in the exception system */
static volatile bool fatal_error_flag = false;

/** flag to determine if the exception system is initialized */
static volatile bool is_initialized = false;

/** flag to determine if the exception system is finalized */
static volatile bool is_finalized = false;

/** main exception context of the program */
static e4c_context main_context = {NULL, NULL, NULL, NULL, NULL};

/** pointer to the current exception context */
static e4c_context * current_context = NULL;


E4C_DEFINE_EXCEPTION(RuntimeException,                  "Runtime exception.",               RuntimeException);
E4C_DEFINE_EXCEPTION(NotEnoughMemoryException,          "Not enough memory.",               RuntimeException);
E4C_DEFINE_EXCEPTION(NullPointerException,              "Null pointer.",                    RuntimeException);

static E4C_DEFINE_EXCEPTION(ExceptionSystemFatalError,  "The exception context for this program is in an invalid state.",   RuntimeException);
static E4C_DEFINE_EXCEPTION(ContextAlreadyBegun,        "The exception context for this program has already begun.",        ExceptionSystemFatalError);
static E4C_DEFINE_EXCEPTION(ContextHasNotBegunYet,      "The exception context for this program has not begun yet.",        ExceptionSystemFatalError);
static E4C_DEFINE_EXCEPTION(ContextNotEnded,            "The program did not end its exception context properly.",          ExceptionSystemFatalError);



static void library_initialize(void);
static void library_finalize(void);
static noreturn void library_panic(
    const e4c_exception_type *  exception_type,
    const char *                message,
    const char *                file,
    int                         line,
    const char *                function,
    int                         error_number
);

static void context_handle_uncaught_exception(const e4c_context * context, const e4c_exception * exception);
static noreturn void context_propagate_exception(const e4c_context * context, e4c_exception * exception);

static e4c_frame * frame_allocate(int line, const char * function);
static void frame_deallocate(e4c_frame * frame, e4c_finalize_handler finalize_handler);
static void frame_initialize(e4c_frame * frame, e4c_frame * previous, enum e4c_frame_stage stage);
static bool exception_type_extends(const e4c_exception_type * child, const e4c_exception_type * parent);

static e4c_exception * exception_allocate();
static void exception_deallocate(e4c_exception * exception, e4c_finalize_handler finalize_handler);
static void exception_initialize(
    e4c_exception *             exception,
    const e4c_exception_type *  exception_type,
    bool                        set_message,
    const char *                message,
    const char *                file,
    int                         line,
    const char *                function,
    int                         error_number
);
static void exception_set_cause(e4c_exception * exception, e4c_exception * cause);
static void exception_print(const char * prefix, const e4c_exception * exception);




/* LIBRARY
 ================================================================ */

static void library_initialize(void) {
    if (!is_initialized) {

        /* registers the function library_finalize to be called when the program exits */
        is_initialized  = (atexit(library_finalize) == 0);
        is_finalized    = !is_initialized;
    }
}

static void library_finalize(void) {

    /* check flag to prevent from looping */
    if (is_finalized) {
        return;
    }

    is_finalized = true;

    /* check for dangling context */
    if (!fatal_error_flag && current_context != NULL) {

        e4c_exception exception;

        /* create temporary exception to be printed out */
        exception_initialize(&exception, &ContextNotEnded, true, NULL, __FILE__, __LINE__, __func__, errno);
        context_handle_uncaught_exception(E4C_CURRENT_CONTEXT, &exception);

        fatal_error_flag = true;
    }

# ifndef NDEBUG
    /* check for critical errors */
    if (fatal_error_flag) {
        /* print fatal error message */
        fprintf(stderr, "\n\nException system errors occurred during program execution.\n");
        (void) fflush(stderr);
    }
# endif

}

static void library_panic(const e4c_exception_type * exception_type, const char * message, const char * file, int line, const char * function, int error_number) {

    e4c_exception exception;

    exception_initialize(&exception, exception_type, true, message, file, line, function, error_number);

    /* ensures library initialization so that library_finalize will be called */
    INITIALIZE_ONCE;

    /* prints this specific exception */
    context_handle_uncaught_exception(E4C_CURRENT_CONTEXT, &exception);

    /* records critical error so that an error message will be printed at exit */
    fatal_error_flag = true;

    exit(EXIT_FAILURE);
}

int e4c_library_version(void) {

    return EXCEPTIONS4C_VERSION;
}

/* CONTEXT
 ================================================================ */

static void context_propagate_exception(const e4c_context * context, e4c_exception * exception) {

    assert(exception != NULL);
    assert(context != NULL);

    e4c_frame * frame = context->current_frame;

    /* if this is the upper frame, then this is an uncaught exception */
    if (frame == NULL) {

        context_handle_uncaught_exception(context, exception);

        e4c_context_end();

        exit(EXIT_FAILURE);
    }

    /* update the frame with the exception information */
    frame->uncaught         = true;

    /* deallocate previously thrown exception */
    exception_deallocate(frame->thrown_exception, context->finalize_handler);

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

/* e4c_context_begin (single-thread) */
void e4c_context_begin() {

    INITIALIZE_ONCE;

    /* check if e4c_context_begin was called twice for this program */
    if (current_context != NULL) {
        library_panic(&ContextAlreadyBegun, NULL, E4C_DEBUG_INFO, errno);
    }

    /* make sure the current frame is not null */
    assert(main_context.current_frame == NULL);

    /* update global variable */
    current_context = &main_context;

    /* initialize context, register uncaught handler */
    current_context->uncaught_handler   = e4c_print_exception;
    current_context->custom_data        = NULL;
    current_context->initialize_handler = NULL;
    current_context->finalize_handler   = NULL;
    current_context->current_frame      = NULL;
}

/* e4c_context_end (single-thread) */
void e4c_context_end(void) {

    e4c_context *       context;

    /* get the current context */
    context = current_context;

    /* make sure `e4c_context_begin` was called before */
    if (context == NULL) {
        library_panic(&ContextHasNotBegunYet, NULL, E4C_DEBUG_INFO, errno);
    }

    /* check if there are too many frames left (breaking out of a try block) */
    if (context->current_frame != NULL) {
        /* deallocate the dangling frames */
        frame_deallocate(context->current_frame, context->finalize_handler);
        library_panic(&ExceptionSystemFatalError, "There are too many exception frames. Probably some try{...} block was exited through 'return' or 'break'.", __FILE__, __LINE__, __func__, errno);
    }

    /* deactivate the current context */
    current_context = NULL;
}

static void context_handle_uncaught_exception(const e4c_context * context, const e4c_exception * exception) {

    assert(exception != NULL);

    if (context == NULL) {

        /* fatal error (likely library misuse) */
        exception_print("\n", exception);
        (void) fflush(stderr);

    } else {

        e4c_uncaught_handler handler;

        handler = context->uncaught_handler;

        if (handler != NULL) {
            handler(exception);
        }
    }
}

void e4c_context_set_handlers(e4c_uncaught_handler uncaught_handler, void * custom_data, e4c_initialize_handler initialize_handler, e4c_finalize_handler finalize_handler) {

    e4c_context * context;

    context = E4C_CURRENT_CONTEXT;

    /* ensure that `e4c_context_begin` was called before */
    if (context == NULL) {
        library_panic(&ContextHasNotBegunYet, NULL, E4C_DEBUG_INFO, errno);
    }

    context->uncaught_handler   = uncaught_handler;
    context->custom_data        = custom_data;
    context->initialize_handler = initialize_handler;
    context->finalize_handler   = finalize_handler;
}

bool e4c_context_is_ready(void) {

    return E4C_CURRENT_CONTEXT != NULL;
}

/* FRAME
 ================================================================ */

e4c_jump_buffer * e4c_start(enum e4c_frame_stage stage, const char * file, int line, const char * function) {

    e4c_context *   context;
    e4c_frame *     current_frame;
    e4c_frame *     new_frame;

    context = E4C_CURRENT_CONTEXT;

    /* check if 'try' was used before calling e4c_context_begin */
    if (context == NULL) {
        library_panic(&ContextHasNotBegunYet, NULL, file, line, function, errno);
    }

    current_frame = context->current_frame;

    /* create a new frame */
    new_frame = frame_allocate(__LINE__, __func__);

    frame_initialize(new_frame, current_frame, stage);

    /* make it the new current frame */
    context->current_frame = new_frame;

    return &(new_frame->continuation);
}

static void frame_initialize(e4c_frame * frame, e4c_frame * previous, enum e4c_frame_stage stage) {

    assert(frame != NULL);

    assert(frame->previous == NULL);
    frame->previous             = previous;
    frame->stage                = stage;
    frame->uncaught             = false;
    frame->reacquire_attempts   = 0;
    frame->retry_attempts       = 0;
    frame->thrown_exception     = NULL;

    /* jmp_buf is an implementation-defined type */
}

static e4c_frame * frame_allocate(int line, const char * function) {

    e4c_frame * frame = calloc(1, sizeof(e4c_frame));

    if (frame == NULL) {
        library_panic(&NotEnoughMemoryException, "Could not create a new exception frame.", __FILE__, line, function, errno);
    }

    return frame;
}

static void frame_deallocate(e4c_frame * frame, e4c_finalize_handler finalize_handler) {

    if (frame != NULL) {

        /* delete previous frame */
        frame_deallocate(frame->previous, finalize_handler);
        frame->previous = NULL;

        /* delete thrown exception */
        exception_deallocate(frame->thrown_exception, finalize_handler);
        frame->thrown_exception = NULL;

        free(frame);
    }
}

enum e4c_frame_stage e4c_get_current_stage(const char * file, int line, const char * function) {

    e4c_context * context;

    context = E4C_CURRENT_CONTEXT;

    /* check if 'e4c_get_current_stage' was used before calling e4c_context_begin */
    if (context == NULL) {
        library_panic(&ContextHasNotBegunYet, NULL, file, line, function, errno);
    }

    /* make sure the current frame is not null */
    assert(context->current_frame != NULL);

    return context->current_frame->stage;
}

bool e4c_catch(const e4c_exception_type * exception_type, const char * file, int line, const char * function) {

    e4c_context * context = E4C_CURRENT_CONTEXT;

    /* ensure that 'e4c_catch' was used after calling e4c_context_begin */
    if (context == NULL) {
        library_panic(&ContextHasNotBegunYet, NULL, file, line, function, errno);
    }

    /* passing NULL to a catch block is considered a fatal error */
    if (exception_type == NULL) {
        library_panic(&NullPointerException, "e4c_catch: A NULL argument was passed.", file, line, function, errno);
    }

    if (context->current_frame->stage != e4c_catching) {
        return false;
    }

    assert(context->current_frame != NULL);
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

    e4c_context *   context;
    e4c_frame *     frame;
    e4c_frame *     previous;
    e4c_exception * thrown_exception;

    context = E4C_CURRENT_CONTEXT;

    /* make sure the current exception context is not null */
    assert(context != NULL);

    /* make sure the current frame is not null */
    assert(context->current_frame != NULL);

    frame = context->current_frame;

    frame->stage++;

    /* simple optimization */
    if (frame->stage == e4c_catching && (!frame->uncaught || frame->thrown_exception == NULL || frame->thrown_exception->type == NULL)) {
        /* if no exception was thrown, or if the thrown exception cannot be
            caught, we don't need to go through the "catching" stage */
        frame->stage++;
    }

    /* keep looping until we reach the "done" stage */
    if (frame->stage < e4c_done) {
        return true;
    }

    /* the exception loop is finished */

    /* deallocate caught exception */
    if (frame->thrown_exception != NULL && !frame->uncaught) {
        exception_deallocate(frame->thrown_exception, context->finalize_handler);
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
    frame_deallocate(frame, context->finalize_handler);

    /* promote the previous frame to the current one */
    context->current_frame = previous;

    /* if the current frame has an uncaught exception, then we will propagate it */
    if (thrown_exception != NULL) {
        context_propagate_exception(context, thrown_exception);
    }
    /* otherwise, we're free to go */

    /* get out of the exception loop */
    return false;
}

void e4c_restart(int max_repeat_attempts, enum e4c_frame_stage stage, const char * file, int line, const char * function) {

    e4c_context *       context;
    e4c_frame *         frame;

    /* get the current context */
    context = E4C_CURRENT_CONTEXT;

    /* check if 'e4c_restart' was used before calling e4c_context_begin */
    if (context == NULL) {
        library_panic(&ContextHasNotBegunYet, NULL, file, line, function, errno);
    }

    /* get the current frame */
    frame = context->current_frame;

    /* check if 'e4c_restart' was used before 'try' or 'use' */
    if (frame == NULL) {
        if (stage == e4c_beginning) {
            library_panic(&ExceptionSystemFatalError, "No E4C_WITH block to reacquire.", file, line, function, errno);
        }
        library_panic(&ExceptionSystemFatalError, "No E4C_TRY block to retry.", file, line, function, errno);
    }

    /* check if "uncatchable" exception */
    if (frame->uncaught && frame->thrown_exception != NULL && frame->thrown_exception->type == NULL) {
        return;
    }

    /* check if maximum number of attempts reached and update the number of attempts */
    switch (stage) {

        case e4c_beginning:
            /* reacquire */
            if (frame->reacquire_attempts >= max_repeat_attempts) {
                return;
            }
            frame->reacquire_attempts++;
            break;

        case e4c_acquiring:
            /* retry */
            if (frame->retry_attempts >= max_repeat_attempts) {
                return;
            }
            frame->retry_attempts++;
            break;

        case e4c_trying:
        case e4c_disposing:
        case e4c_catching:
        case e4c_finalizing:
        case e4c_done:
        default:
            library_panic(&ExceptionSystemFatalError, "The specified stage can't be repeated.", file, line, function, errno);
    }

    /* deallocate previously thrown exception */
    exception_deallocate(frame->thrown_exception, context->finalize_handler);

    /* reset exception information */
    frame->thrown_exception = NULL;
    frame->uncaught         = false;
    frame->stage            = stage;

    /* keep looping */
    EXCEPTIONS4C_LONG_JUMP(frame->continuation);
}

e4c_status e4c_get_status(void) {

    e4c_context *   context;
    e4c_frame *     frame;

    context = E4C_CURRENT_CONTEXT;

    /* ensure that `e4c_get_status` was called after calling `e4c_context_begin` */
    if (context != NULL) {

        /* make sure the current frame is not null */
        assert(context->current_frame != NULL);

        frame = context->current_frame;

        if (frame->thrown_exception == NULL) {
            return e4c_succeeded;
        }

        if (frame->uncaught) {
            return e4c_failed;
        }

        return e4c_recovered;
    }

    library_panic(&ContextHasNotBegunYet, NULL, E4C_DEBUG_INFO, errno);
}

/* EXCEPTION TYPE
 ================================================================ */

static bool exception_type_extends(const e4c_exception_type * child, const e4c_exception_type * parent) {

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

bool e4c_is_instance_of(const e4c_exception * instance, const e4c_exception_type * exception_type) {

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

const e4c_exception * e4c_get_exception(void) {

    e4c_context *   context;

    context = E4C_CURRENT_CONTEXT;

    /* check if `e4c_get_exception` was called before calling `e4c_context_begin` */
    if (context == NULL) {
        library_panic(&ContextHasNotBegunYet, NULL, E4C_DEBUG_INFO, errno);
    }

    /* make sure the current frame is not null */
    assert(context->current_frame != NULL);

    return context->current_frame->thrown_exception;
}

void e4c_throw(const e4c_exception_type * exception_type, const char * file, int line, const char * function, const char * format, ...) {

    int                 error_number;
    e4c_context *       context;
    e4c_frame *         frame;
    e4c_exception *     new_exception;

    /* store the current error number up front */
    error_number = errno;

    /* get the current context */
    context = E4C_CURRENT_CONTEXT;

    /* make sure e4c_context_begin was called before */
    if (context == NULL) {
        library_panic(&ContextHasNotBegunYet, NULL, file, line, function, errno);
    }

    /* check context and frame; initialize exception and cause */
    new_exception = exception_allocate();

    /* convert NULL exception type to NPE */
    if (exception_type == NULL) {
        exception_type = &NullPointerException;
    }

    /* "instantiate" the specified exception */
    exception_initialize(new_exception, exception_type, format == NULL, NULL, file, line, function, error_number);

    /* get the current frame */
    frame = context->current_frame;

    /* capture the cause of this exception */
    while (frame != NULL) {
        if (frame->thrown_exception != NULL) {
            exception_set_cause(new_exception, frame->thrown_exception);
            break;
        }
        frame = frame->previous;
    }

    /* format the message */
    if (format != NULL) {
        va_list arguments_list;
        va_start(arguments_list, format);
        (void) vsnprintf(new_exception->message, E4C_EXCEPTION_MESSAGE_SIZE, format, arguments_list);
        va_end(arguments_list);
    }

    /* set initial value for custom data */
    new_exception->custom_data = context->custom_data;
    /* initialize custom data */
    if (context->initialize_handler != NULL) {
        new_exception->custom_data = context->initialize_handler(new_exception);
    }

    /* propagate the exception up the call stack */
    context_propagate_exception(context, new_exception);
}

static void exception_initialize(e4c_exception * exception, const e4c_exception_type * exception_type, bool set_message, const char * message, const char * file, int line, const char * function, int error_number) {

    assert(exception != NULL);
    assert(exception_type != NULL);

    exception->ref_count    = 1;
    exception->name         = exception_type->name;
    exception->file         = file;
    exception->line         = line;
    exception->function     = function;
    exception->error_number = error_number;
    exception->type         = exception_type;
    exception->cause        = NULL;

    if (set_message) {
        /* initialize the message of this exception */
        if (message != NULL) {
            /* copy the given message */
            VERBATIM_COPY(exception->message, message);
        } else {
            /* copy the default message for this type of exception */
            VERBATIM_COPY(exception->message, exception_type->default_message);
        }
    }
    /*
     * since the exception is allocated and then zero-initialized,
     * there's no need to truncate the message when !set_message.
     */
}

static e4c_exception * exception_allocate() {

    e4c_exception * exception = calloc(1, sizeof(e4c_exception));

    /* make sure there was enough memory */
    if (exception == NULL) {
        library_panic(&NotEnoughMemoryException, "Could not create a new exception.", __FILE__, __LINE__, __func__, errno);
    }

    return exception;
}

static void exception_deallocate(e4c_exception * exception, e4c_finalize_handler finalize_handler) {

    if (exception != NULL) {

        exception->ref_count--;

        if (exception->ref_count <= 0) {

            exception_deallocate(exception->cause, finalize_handler);

            if (finalize_handler != NULL) {
                finalize_handler(exception->custom_data);
            }

            free(exception);
        }
    }
}

static void exception_set_cause(e4c_exception * exception, e4c_exception * cause) {

    assert(exception != NULL);
    assert(cause != NULL);

    exception->cause = cause;

    cause->ref_count++;
}

static void exception_print(const char * prefix, const e4c_exception * exception) {

    assert(exception != NULL);

    fprintf(stderr, "%s%s: %s\n", prefix, exception->name, exception->message);

    if (exception->file != NULL) {
        if (exception->function != NULL) {
            fprintf(stderr, "    at %s (%s:%d)\n", exception->function, exception->file, exception->line);
        } else {
            fprintf(stderr, "    at %s:%d\n", exception->file, exception->line);
        }
    }

    if (exception->cause != NULL) {
        exception_print("Caused by: ", exception->cause);
    }
}

void e4c_print_exception(const e4c_exception * exception) {

    if (exception == NULL) {
        fprintf(stderr, "No exception\n");
    } else {
        exception_print("\n", exception);
    }

    (void) fflush(stderr);
}
