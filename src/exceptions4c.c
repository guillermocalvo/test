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
 * Implementation of the exception handling library for C.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * @file        exceptions4c.c
 * @version     4.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <exceptions4c.h>

/**
 * @internal
 * @brief Represents the execution stage of the current exception block.
 */
enum block_stage {

    /** @internal The exception block has started. */
    BEGINNING,

    /** @internal The exception block is [acquiring a resource](#WITH). */
    ACQUIRING,

    /** @internal The exception block is [trying something](#TRY) or [using a resource](#USE). */
    TRYING,

    /** @internal The exception block is [disposing of a resource](#WITH). */
    DISPOSING,

    /** @internal The exception block is [catching an exception](#CATCH). */
    CATCHING,

    /** @internal The exception block is [finalizing](#FINALLY). */
    FINALIZING,

    /** @internal The exception block has finished. */
    DONE
};

/**
 * @internal
 * @brief Represents an exception block.
 */
struct e4c_block {

    /** A possibly-null pointer to the outer exception block. */
    struct e4c_block * outer_block;

    /** The stage of this block. */
    enum block_stage stage;

    /** Whether this block currently has an uncaught exception. */
    bool uncaught;

    /** A possibly-null pointer to the currently thrown exceptions. */
    struct e4c_exception * exception;

    /** Current number of times the #TRY block has been attempted. */
    int retry_attempts;

    /** Current number of times the #WITH block has been attempted. */
    int reacquire_attempts;

    /** The execution context of this exception block. */
    e4c_env env;
};

static noreturn void panic(const char * error_message, const char * file, int line, const char * function);
static void * allocate(size_t size, const char * error_message, const char * file, int line, const char * function);
static struct e4c_context * get_context(const char * file, int line, const char * function);
static void cleanup_default_context(void);
static void throw(const struct e4c_context * context, const struct e4c_exception_type * type, const char * name, int error_number, const char * file, int line, const char * function, const char * format, va_list arguments_list);
static void propagate(const struct e4c_context * context, struct e4c_exception * exception);
static enum block_stage get_stage(const char * file, int line, const char * function);
static void delete_exception(const struct e4c_context * context, struct e4c_exception * exception);
static void print_debug_info(const char * file, int line, const char * function);
static void print_exception(const struct e4c_exception * exception, bool is_cause);
static bool extends(const struct e4c_exception_type * type, const struct e4c_exception_type * supertype);

/** Stores the exception context supplier. */
static struct e4c_context * (*context_supplier)(void) = NULL;

/** Default exception context of the program when no custom supplier is provided. */
static struct e4c_context default_context = {
    ._innermost_block = NULL,
    .initialize_exception = NULL,
    .finalize_exception = NULL,
    .uncaught_handler = NULL
};

/** Flag that determines if the exception system has been already initialized. */
static bool is_cleanup_registered = false;

void e4c_set_context_supplier(struct e4c_context * (*supplier)(void)) {
    context_supplier = supplier;
}

struct e4c_context * e4c_get_context(void) {
    return context_supplier != NULL ? context_supplier() : &default_context;
}

const struct e4c_exception * e4c_get_exception(void) {
    const struct e4c_context * context = e4c_get_context();
    return context != NULL && context->_innermost_block != NULL ? ((struct e4c_block *) context->_innermost_block)->exception : NULL;
}

bool e4c_is_uncaught(void) {
    const struct e4c_context * context = e4c_get_context();
    return context != NULL && context->_innermost_block != NULL && ((struct e4c_block *) context->_innermost_block)->uncaught;
}

e4c_env * e4c_start(const bool should_acquire, const char * file, const int line, const char * function) {
    struct e4c_block * new_block = allocate(sizeof(*new_block), "Not enough memory to create a new exception block", file, line, function);
    struct e4c_context * context = get_context(file, line, function);
    if (context == &default_context && !is_cleanup_registered) {
        if (atexit(cleanup_default_context) != 0) {
            panic("Cleanup function could not be registered.", file, line, function);
        }
        is_cleanup_registered = true;
    }

    new_block->outer_block          = context->_innermost_block;
    new_block->stage                = should_acquire ? BEGINNING : ACQUIRING;
    new_block->uncaught             = false;
    new_block->reacquire_attempts   = 0;
    new_block->retry_attempts       = 0;
    new_block->exception            = NULL;

    context->_innermost_block = new_block;

    return &new_block->env;
}

bool e4c_next(const char * file, const int line, const char * function) {
    struct e4c_context * context = get_context(file, line, function);
    struct e4c_block * block = context->_innermost_block;
    if (block == NULL) {
        panic("Invalid exception context state.", file, line, function);
    }

    /* advance the block to the next stage */
    block->stage++;

    struct e4c_exception * exception = block->exception;
    const bool uncaught = block->uncaught;

    /* simple optimization: if no exception was thrown, CATCHING stage can be skipped */
    if (block->stage == CATCHING && (exception == NULL || !uncaught)) {
        block->stage++;
    }

    /* carry on until the block is DONE */
    if (block->stage < DONE) {
        return true;
    }

    /* deallocate this block and promote its outer block to be the current one */
    context->_innermost_block = block->outer_block;
    free(block);

    /* deallocate or propagate its exception, depending on whether it was caught */
    if (exception != NULL) {
        if (uncaught) {
            propagate(context, exception);
        } else {
            delete_exception(context, exception);
        }
    }

    /* get out of the loop */
    return false;
}

e4c_env * e4c_get_env(void) {
    const struct e4c_context * context = e4c_get_context();
    return context != NULL && context->_innermost_block != NULL ? &((struct e4c_block *) context->_innermost_block)->env : NULL;
}

bool e4c_acquire(const char * file, const int line, const char * function) {
    return get_stage(file, line, function) == ACQUIRING;
}

bool e4c_try(const char * file, const int line, const char * function) {
    return get_stage(file, line, function) == TRYING;
}

bool e4c_dispose(const char * file, const int line, const char * function) {
    return get_stage(file, line, function) == DISPOSING;
}

bool e4c_catch(const struct e4c_exception_type * type, const char * file, const int line, const char * function) {
    const struct e4c_context * context = get_context(file, line, function);
    struct e4c_block * block = context->_innermost_block;
    if (block == NULL) {
        panic("Invalid exception context state.", file, line, function);
    }
    /* check if the exception can be handled given the supplied exception type */
    if (block->stage == CATCHING
        && block->exception != NULL && block->uncaught
        && (type == NULL || extends(block->exception->type, type))) {
        block->uncaught = false;
        return true;
    }
    return false;
}

bool e4c_finally(const char * file, const int line, const char * function) {
    return get_stage(file, line, function) == FINALIZING;
}

e4c_env * e4c_throw(const struct e4c_exception_type * type, const char * name, const char * file, const int line, const char * function, const char * format, ...) {
    const int error_number = errno;
    const struct e4c_context * context = get_context(file, line, function);

    va_list arguments_list;
    va_start(arguments_list, format);
    throw(context, type, name, error_number, file, line, function, format, arguments_list);
    va_end(arguments_list);

    return &((struct e4c_block *) context->_innermost_block)->env;
}

e4c_env * e4c_restart(const bool should_reacquire, const int max_attempts, const struct e4c_exception_type * type, const char * name, const char * file, const int line, const char * function, const char * format, ...) {
    const int error_number = errno;
    const struct e4c_context * context = get_context(file, line, function);
    struct e4c_block * block = context->_innermost_block;
    if (block == NULL) {
        panic(should_reacquire ? "No `WITH` block to reacquire." : "No `TRY` block to retry.", file, line, function);
    }

    /* check if maximum number of attempts reached and update the number of attempts */
    bool max_reached;
    if (should_reacquire) {
        max_reached = block->reacquire_attempts >= max_attempts;
        if (!max_reached) {
            block->reacquire_attempts++;
        }
    } else {
        max_reached = block->retry_attempts >= max_attempts;
        if (!max_reached) {
            block->retry_attempts++;
        }
    }

    if (max_reached) {
        /* throw a new exception, possibly using the current one as the cause of the new one */
        va_list arguments_list;
        va_start(arguments_list, format);
        throw(context, type, name, error_number, file, line, function, format, arguments_list);
        va_end(arguments_list);
    } else {
        /* suppress the currently thrown exception; jump back to the TRY or WITH block */
        if (block->exception != NULL) {
            delete_exception(context, block->exception);
            block->exception = NULL;
        }
        block->uncaught     = false;
        block->stage        = should_reacquire ? BEGINNING : ACQUIRING;
    }

    return &block->env;
}

/**
 * Causes abnormal program termination due to a fatal error.
 *
 * @param error_message The message to print to standard error output.
 * @param file the name of the client source code file that caused the fatal error.
 * @param line the number of line that caused the fatal error.
 * @param function the name of the client function that caused the fatal error.
 */
static noreturn void panic(const char * error_message, const char * file, const int line, const char * function) {
    fprintf(stderr, "[exceptions4c] %s\n", error_message);
    print_debug_info(file, line, function);
    fflush(stderr);
    abort();
}

/**
 * Allocates memory for a object of size and initializes all bytes in the allocated storage to zero.
 *
 * @param size the size of the new object.
 * @param error_message The message to print to standard error output.
 * @param file the name of the client source code file that caused the fatal error.
 * @param line the number of line that caused the fatal error.
 * @param function the name of the client function that caused the fatal error.
 * @return a pointer to the newly allocated memory.
 */
static void * allocate(size_t size, const char * error_message, const char * file, int line, const char * function) {
    void * object = calloc(1, size);
    if (object == NULL) {
        panic(error_message, file, line, function);
    }
    return object;
}

/**
 * Checks for dangling exception blocks at program exit.
 */
static void cleanup_default_context(void) {
    if (default_context._innermost_block != NULL) {
        panic("Dangling exception block leaked. Some `TRY` block may have been exited improperly (via `goto`, `break`, `continue`, or `return`).", NULL, 0, NULL);
    }
}

/**
 * Retrieves the current exception context; <em>panics</em> if <tt>NULL</tt>.
 *
 * @param file the name of the client source code file that requires the exception context.
 * @param line the number of line that requires the exception context.
 * @param function the name of the client function that requires the exception context.
 * @return a non-null pointer to the current exception context.
 */
static struct e4c_context * get_context(const char * file, const int line, const char * function) {
    struct e4c_context * context = e4c_get_context();
    if (context == NULL) {
        panic("Context supplier returned NULL.", file, line, function);
    }
    return context;
}

/**
 * Propagates the supplied exception in the supplied context.
 *
 * @param context the context whose innermost exception block will receive the exception.
 * @param exception the exception to propagate.
 *
 * @note
 * If the exception reached the top level of the program, then the program will be abruptly terminated (after calling the uncaught handler).
 */
static void propagate(const struct e4c_context * context, struct e4c_exception * exception) {
    struct e4c_block * block = context->_innermost_block;
    if (block == NULL) {
        /* uncaught exception handler */
        if (context->uncaught_handler != NULL) {
            context->uncaught_handler(exception);
        } else {
            print_exception(exception, false);
            (void) fflush(stderr);
        }
        /* delete the exception to avoid memory leaks */
        delete_exception(context, exception);
        /* abrupt termination handler */
        if (context->termination_handler != NULL) {
            context->termination_handler();
            return;
        }
        exit(EXIT_FAILURE);
    }

    /** if the block already had an exception, it will be suppressed by the new one */
    if (block->exception != NULL && block->exception != exception) {
        delete_exception(context, block->exception);
    }

    block->exception = exception;
    block->uncaught = true;

    /* simple optimization: if we were ACQUIRING a resource, there's no need to dispose of it */
    if (block->stage == ACQUIRING) {
        block->stage = DISPOSING;
    }
}

/**
 *
 * @param file
 * @param line
 * @param function
 * @return
 */
static enum block_stage get_stage(const char * file, const int line, const char * function) {

    const struct e4c_context * context = get_context(file, line, function);
    const struct e4c_block * block = context->_innermost_block;

    if (block == NULL) {
        panic("Invalid exception context state.", file, line, function);
    }

    return block->stage;
}

/**
 *
 * @param type
 * @param supertype
 * @return
 */
static bool extends(const struct e4c_exception_type * type, const struct e4c_exception_type * supertype) {
    for (; type != NULL; type = type != type->supertype ? type->supertype : NULL) {
        if (type == supertype) {
            return true;
        }
    }
    return false;
}

/**
 *
 * @param context
 * @param type
 * @param name
 * @param error_number
 * @param file
 * @param line
 * @param function
 * @param format
 * @param arguments_list
 */
static void throw(const struct e4c_context * context, const struct e4c_exception_type * type, const char * name, int error_number, const char * file, const int line, const char * function, const char * format, va_list arguments_list) {

    /* allocate new exception */
    struct e4c_exception * exception = allocate(sizeof(*exception), "Not enough memory to create a new exception", file, line, function);

    /* "instantiate" the specified exception */
    exception->name         = name;
    exception->file         = file;
    exception->line         = line;
    exception->function     = function;
    exception->error_number = error_number;
    exception->type         = type;
    exception->cause        = NULL;
    exception->data         = NULL;

    if (format == NULL && type != NULL) {
        (void) snprintf(exception->message, sizeof(exception->message), "%s", type->default_message);
    } else if (format != NULL) {
        (void) vsnprintf(exception->message, sizeof(exception->message), format, arguments_list);
    }

    /* capture the cause of this exception */
    for (struct e4c_block * block = context->_innermost_block; block != NULL; block = block->outer_block) {
        if (block->exception != NULL && (block->uncaught || block->stage == CATCHING)) {
            exception->cause = block->exception;
            block->exception = NULL;
            break;
        }
    }

    /* initialize custom data */
    if (context->initialize_exception != NULL) {
        context->initialize_exception(exception);
    }

    propagate(context, exception);
}

/**
 * Deletes the supplied exception, along with its cause.
 *
 * @param context the context the supplied exception belongs to.
 * @param exception the exception to delete.
 */
static void delete_exception(const struct e4c_context * context, struct e4c_exception * exception) {
    if (context->finalize_exception != NULL) {
        context->finalize_exception(exception);
    }
    if (exception->cause != NULL) {
        delete_exception(context, exception->cause);
    }
    free(exception);
}

/**
 * Prints debug info (if available) to the standard error output.
 *
 * @param file the name of the source code file.
 * @param line the number of line.
 * @param function the name of the function.
 */
static void print_debug_info(const char * file, const int line, const char * function) {
    if (file != NULL) {
        (void) fprintf(stderr, "    at %s (%s:%d)\n", function, file, line);
    }
}

/**
 * Prints the supplied exception to the standard error output.
 *
 * @param exception the exception to print.
 * @param is_cause <tt>true</tt> if the supplied exception is the cause of another one.
 */
static void print_exception(const struct e4c_exception * exception, const bool is_cause) {
    (void) fprintf(stderr, "%s%s: %s\n", is_cause ? "Caused by: " : "\n", exception->name, exception->message);
    print_debug_info(exception->file, exception->line, exception->function);
    if (exception->cause != NULL) {
        print_exception(exception->cause, true);
    }
}
