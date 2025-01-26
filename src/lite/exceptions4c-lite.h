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
 * Really lightweight exception handling for C.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * This is the lightweight version of the library.
 *
 * @file        exceptions4c-lite.h
 * @version     4.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#ifndef EXCEPTIONS4C_LITE

/**
 * Returns the major version number of this library.
 */
#define EXCEPTIONS4C_LITE 4

#include <setjmp.h> /* longjmp, setjmp */
#include <stdio.h> /* stderr, fprintf, fflush, sprintf */
#include <stdlib.h> /* abort,  EXIT_FAILURE, exit */

#ifndef EXCEPTIONS4C_MAX_BLOCKS

/**
 * @internal
 * @brief Maximum number of #TRY blocks that can be nested.
 */
#define EXCEPTIONS4C_MAX_BLOCKS 32

#endif

/**
 * Represents a category of problematic situations in a program.
 *
 * #e4c_exception_type is used to define a kind of error or exceptional
 * condition that a program might want to #THROW and #CATCH. It serves as
 * a way to group related issues that share common characteristics.
 *
 * Exception types SHOULD be defined as <tt>const</tt>.
 *
 * ```c
 * const struct e4c_exception_type IO_ERROR = {"I/O Error"};
 * ```
 *
 * @see #THROW
 * @see #CATCH
 */
struct e4c_exception_type {

    /** A default message that summarizes the kind of error or issue. */
    const char * default_message;
};

/**
 * Represents a specific occurrence of an exceptional situation in a
 * program.
 *
 * #e4c_exception ties a specific instance of an exception to its type.
 * It combines an #e4c_exception_type (which defines the general category
 * of the exception) with a detailed error message that provides specific
 * information about what went wrong in this particular instance.
 *
 * After an exception is [thrown](#THROW), it may propagate through the
 * program and be caught by an appropriate #CATCH or #CATCH_ALL block.
 * When an exception is caught, #THROWN_EXCEPTION can be used to retrieve
 * the exception currently being handled. This allows for inspection and
 * further handling of the error based on both its type and the detailed
 * context of the situation.
 */
struct e4c_exception {

    /** The general nature of the error. */
    const struct e4c_exception_type * type;

    /** The name of the exception type. */
    const char * name;

    /** A text message describing the specific problem. */
    char message[256];

    /** The name of the source code file that threw this exception. */
    const char * file;

    /** The number of line that threw this exception. */
    int line;
};

/**
 * @internal
 * @brief Represents the current status of exceptions.
 */
struct e4c_context {
  unsigned char blocks;
  struct e4c_exception exception;
  struct {
    unsigned char stage;
    unsigned char uncaught;
    jmp_buf jump;
  } block[EXCEPTIONS4C_MAX_BLOCKS];
};

/**
 * Contains the current status of exceptions.
 *
 * You MUST define this global variable for your program.
 *
 * ```c
 * struct e4c_context exceptions4c = {0};
 * ```
 */
extern struct e4c_context exceptions4c;

/**
 * @internal
 * @brief Represents the execution stage of the current exception block.
 */
enum e4c_stage {

  /** @internal The exception block has started. */
  EXCEPTIONS4C_START,

  /** @internal The exception block is [trying something](#TRY). */
  EXCEPTIONS4C_TRY,

  /** @internal The exception block is [catching an exception](#CATCH). */
  EXCEPTIONS4C_CATCH,

  /** @internal The exception block is [finalizing](#FINALLY). */
  EXCEPTIONS4C_FINALLY,

  /** @internal The exception block has finished. */
  EXCEPTIONS4C_DONE
};

/**
 * Introduces a block of code that may throw exceptions during execution.
 *
 * The #TRY block is used to define a section of code where exceptions
 * might occur. It allows you to handle exceptions gracefully using other
 * blocks that follow it. If an exception occurs, control is transferred
 * to the appropriate block.
 *
 * A single #TRY block can be followed by:
 *
 * - One or more #CATCH blocks to handle specific types of exceptions.
 * - Optionally, one #CATCH_ALL block to handle all exception types (if
 *   present, it must appear after all #CATCH blocks).
 * - Optionally, one #FINALLY block to execute cleanup code, regardless
 *   of whether an exception was thrown or caught.
 *
 * @note
 * The blocks must appear in this order: #CATCH blocks (if any),
 * #CATCH_ALL block (if any), and #FINALLY block (if any). A single #TRY
 * block MUST be followed by, at least, one #CATCH, #CATCH_ALL, or
 * #FINALLY block.
 *
 * @see CATCH
 * @see CATCH_ALL
 * @see FINALLY
 */
#define TRY                                                                 \
                                                                            \
  for (                                                                     \
    (void) (                                                                \
      exceptions4c.blocks >= EXCEPTIONS4C_MAX_BLOCKS && (                   \
        (void) fprintf(stderr,                                              \
          "\n[exceptions4c]: Too many `try` blocks nested.\n    at %s:%d\n",\
          __FILE__, __LINE__                                                \
        ),                                                                  \
        (void) fflush(stderr),                                              \
        abort(), 0                                                          \
      )                                                                     \
    ),                                                                      \
    exceptions4c.block[exceptions4c.blocks].stage = EXCEPTIONS4C_START,     \
    exceptions4c.block[exceptions4c.blocks].uncaught = 0,                   \
    exceptions4c.blocks++,                                                  \
    (void) setjmp(exceptions4c.block[exceptions4c.blocks - 1].jump);        \
                                                                            \
    ++exceptions4c.block[exceptions4c.blocks - 1].stage < EXCEPTIONS4C_DONE \
    || (                                                                    \
      exceptions4c.block[--exceptions4c.blocks].uncaught && (               \
        (void) (                                                            \
          exceptions4c.blocks > 0 && (                                      \
            exceptions4c.block[exceptions4c.blocks - 1].uncaught = 1,       \
            (void) longjmp(                                                 \
              exceptions4c.block[exceptions4c.blocks - 1].jump, 1           \
            ), 0                                                            \
          )                                                                 \
        ),                                                                  \
        (void) fprintf(stderr,                                              \
          "\n%s: %s\n    at %s:%d\n",                                       \
          exceptions4c.exception.name, exceptions4c.exception.message,      \
          exceptions4c.exception.file, exceptions4c.exception.line          \
        ),                                                                  \
        (void) fflush(stderr),                                              \
        exit(EXIT_FAILURE), 0                                               \
      )                                                                     \
    );                                                                      \
  )                                                                         \
    if (                                                                    \
      exceptions4c.block[exceptions4c.blocks - 1].stage == EXCEPTIONS4C_TRY \
    )

/**
 * Introduces a block of code that handles exceptions thrown by a
 * preceding #TRY block.
 *
 * @param exception_type the type of exception to catch.
 *
 * One or more #CATCH blocks can follow a #TRY block. Each #CATCH block
 * MUST specify the type of exception it handles.
 *
 * @see #TRY
 * @see #CATCH_ALL
 *
 */
#define CATCH(exception_type)                                               \
                                                                            \
    else if (                                                               \
      exceptions4c.blocks > 0                                               \
      &&                                                                    \
      exceptions4c.blocks <= EXCEPTIONS4C_MAX_BLOCKS                        \
      &&                                                                    \
      exceptions4c.block[exceptions4c.blocks - 1].stage ==                  \
        EXCEPTIONS4C_CATCH                                                  \
      &&                                                                    \
      exceptions4c.block[exceptions4c.blocks - 1].uncaught == 1             \
      &&                                                                    \
      &(exception_type) == exceptions4c.exception.type                      \
      &&                                                                    \
      (exceptions4c.block[exceptions4c.blocks - 1].uncaught = 0, 1)         \
    )

/**
 * Introduces a block of code that handles any exception thrown by a
 * preceding #TRY block, regardless of its type.
 *
 * The #CATCH_ALL block works like a general #CATCH block that does not
 * require specifying the type of exception to handle. It acts as a
 * fallback for catching all exceptions, including those not explicitly
 * declared in other #CATCH blocks.
 *
 * Only one #CATCH_ALL block is allowed per #TRY block, and it must
 * appear after all type-specific #CATCH blocks if any are present.
 *
 * @remark
 * Using a #CATCH_ALL block is useful for logging, debugging, or handling
 * unexpected exceptions that don't fit into specific categories.
 * However, specific #CATCH blocks SHOULD be used whenever possible to
 * maintain clarity and precise control over exception handling.
 *
 * @see TRY
 * @see CATCH
 */
#define CATCH_ALL                                                           \
                                                                            \
    else if (                                                               \
      exceptions4c.blocks > 0                                               \
      &&                                                                    \
      exceptions4c.blocks <= EXCEPTIONS4C_MAX_BLOCKS                        \
      &&                                                                    \
      exceptions4c.block[exceptions4c.blocks - 1].stage ==                  \
        EXCEPTIONS4C_CATCH                                                  \
      &&                                                                    \
      exceptions4c.block[exceptions4c.blocks - 1].uncaught == 1             \
      &&                                                                    \
      (exceptions4c.block[exceptions4c.blocks - 1].uncaught = 0, 1)         \
    )

/**
 * Introduces a block of code that is executed after a #TRY block,
 * regardless of whether an exception was thrown or not.
 *
 * A #FINALLY block can follow a #TRY block, with or without accompanying
 * #CATCH blocks. Only one #FINALLY block is allowed per #TRY block.
 *
 * @remark
 * It is typically used to release resources, close files, or perform
 * cleanup tasks.
 */
#define FINALLY                                                             \
                                                                            \
    else if (                                                               \
      exceptions4c.blocks > 0                                               \
      &&                                                                    \
      exceptions4c.blocks <= EXCEPTIONS4C_MAX_BLOCKS                        \
      &&                                                                    \
      exceptions4c.block[exceptions4c.blocks - 1].stage ==                  \
        EXCEPTIONS4C_FINALLY                                                \
    )

/**
 * Throws an exception, interrupting the normal flow of execution.
 *
 * @param exception_type the type of the exception to throw.
 * @param format the error message.
 * @param ... an optional list of arguments that will be formatted according to <tt>format</tt>.
 *
 * #THROW is used within a #TRY block, a #CATCH block, or any other
 * function to signal that an error has occurred. The thrown exception
 * will be of the specified type, and it MAY be handled by a preceding
 * #CATCH block.
 *
 * If a thrown exception is not handled by any of the #CATCH blocks in
 * the current function, it propagates up the call stack to the function
 * that called the current function. This continues until the exception
 * is either handled by a #CATCH block higher in the stack, or it reaches
 * the top level of the program. If no #CATCH block handles the
 * exception, the program terminates and an error message is printed to
 * the console.
 */
#define THROW(exception_type, format, ...)                                  \
                                                                            \
  (                                                                         \
    exceptions4c.exception.type = &(exception_type),                        \
    exceptions4c.exception.name = #exception_type,                          \
    exceptions4c.exception.file = (format),                                 \
    (void) snprintf(                                                        \
      exceptions4c.exception.message,                                       \
      sizeof(exceptions4c.exception.message) - 1,                           \
      exceptions4c.exception.file ? exceptions4c.exception.file             \
        : exceptions4c.exception.type->default_message                      \
      __VA_OPT__(,) __VA_ARGS__                                             \
    ),                                                                      \
    exceptions4c.exception.file = __FILE__,                                 \
    exceptions4c.exception.line = __LINE__,                                 \
    (                                                                       \
      exceptions4c.blocks <= 0                                              \
      &&                                                                    \
      (                                                                     \
        (void) fprintf(stderr,                                              \
          "\n%s: %s\n    at %s:%d\n",                                       \
          exceptions4c.exception.name,                                      \
          exceptions4c.exception.message,                                   \
          exceptions4c.exception.file,                                      \
          exceptions4c.exception.line                                       \
        ),                                                                  \
        (void) fflush(stderr),                                              \
        exit(EXIT_FAILURE),                                                 \
        0                                                                   \
      )                                                                     \
    ),                                                                      \
    exceptions4c.block[exceptions4c.blocks - 1].uncaught = 1,               \
    longjmp(exceptions4c.block[exceptions4c.blocks - 1].jump, 1)            \
  )

/**
 * Retrieves the last exception that was thrown.
 *
 * @return the last exception that was thrown.
 *
 * This macro SHOULD be used in the body of a #CATCH or #CATCH_ALL
 * block to inspect the exception being handled.
 *
 * It MAY also be used in the body of a #FINALLY block to determine if an
 * exception was thrown in the corresponding #TRY block, or during the
 * execution of a #CATCH or #CATCH_ALL block.
 *
 * @see #e4c_exception
 * @see #THROW
 * @see #CATCH
 * @see #FINALLY
 * @see #IS_UNCAUGHT
 */
#define THROWN_EXCEPTION                                                    \
                                                                            \
  exceptions4c.exception

/**
 * Determines whether the current exception (if any) hasn't been handled
 * yet by any #CATCH or #CATCH_ALL block.
 *
 * @return <tt>1</tt> if the exception from the #TRY block was not caught
 *   by any #CATCH or #CATCH_ALL block; <tt>0</tt> otherwise.
 *
 * An exception is considered "uncaught" if no matching #CATCH or
 * #CATCH_ALL block has been executed for it. In other words, this macro
 * evaluates to a truthy value if the exception has bypassed all specific
 * exception-handling logic and is propagating further. And it evaluates
 * to a falsy value if no exception was thrown in the #TRY block, or if
 * an exception was successfully caught.
 *
 * @remark
 * This macro SHOULD be used exclusively in the body of a #FINALLY block
 * to check whether an exception thrown during the #TRY block has
 * propagated past all #CATCH and #CATCH_ALL blocks without being
 * handled.
 *
 * @see FINALLY
 * @see THROWN_EXCEPTION
 */
#define IS_UNCAUGHT                                                         \
                                                                            \
  (                                                                         \
    exceptions4c.blocks > 0                                                 \
    &&                                                                      \
    exceptions4c.blocks <= EXCEPTIONS4C_MAX_BLOCKS                          \
    &&                                                                      \
    exceptions4c.block[exceptions4c.blocks - 1].uncaught                    \
  )

/* OpenMP support */
#ifdef _OPENMP
# pragma omp threadprivate(exceptions4c)
#endif

#endif
