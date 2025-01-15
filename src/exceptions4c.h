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
 * A tiny exception handling library for C.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * This library consists of two files:
 * - exceptions4c.h
 * - exceptions4c.c
 *
 * All you need to do is include the header file <exceptions4c.h>, and
 * link your code against the library code.
 *
 * ```c
 * #include <exceptions4c.h>
 * ```
 *
 * @file        exceptions4c.h
 * @version     4.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#ifndef EXCEPTIONS4C

/**
 * Returns the major version number of this library.
 */
#define EXCEPTIONS4C 4

#include <stdlib.h>
#include <setjmp.h>

#ifndef __bool_true_false_are_defined
#include <stdbool.h>
#endif

/**
 * Introduces a block of code aware of exceptions
 *
 * A #TRY statement executes a block of code. If an exception is thrown and
 * there is a #CATCH block that can handle it, then control will be
 * transferred to it. If there is a #FINALLY block, then it will be executed,
 * no matter whether the #TRY block completes normally or abruptly, and no
 * matter whether a #CATCH block is first given control.
 *
 * ```c
 * stack_t * stack = stack_new();
 * TRY {
 *   // the try block
 *   int value = stack_pop(stack);
 *   stack_push(stack, 16);
 *   stack_push(stack, 32);
 * } CATCH(StackOverflowException) {
 *   // a catch block
 *   printf("Could not push.");
 * } CATCH(StackUnderflowException) {
 *   // another catch block
 *   printf("Could not pop.");
 * } FINALLY {
 *   // the finally block
 *   stack_delete(stack);
 * }
 * ```
 *
 * One #TRY block may precede many #CATCH blocks (also called *exception
 * handlers*). A #CATCH block **must** have exactly one parameter, which is the
 * exception type it is capable of handling. Within the #CATCH block, the
 * exception can be accessed through the function #e4c_get_exception.
 * Exception handlers are considered in left-to-right order: the earliest
 * possible #CATCH block handles the exception. If no #CATCH block can handle
 * the thrown exception, it will be *propagated*.
 *
 * Sometimes it may come in handy to #RETRY an entire #TRY block; for
 * instance, once the exception has been caught and the error condition has been
 * solved.
 *
 * @pre
 *   - A #TRY block **must** precede, at least, another block of code,
 *     introduced by either #CATCH or #FINALLY.
 *     - A #TRY block may precede several #CATCH blocks.
 *     - A #TRY block can precede, at most, one #FINALLY block.
 *   - A #TRY block **must not** be exited through any of: <tt>goto</tt>, <tt>break</tt>,
 *     <tt>continue</tt>, or <tt>return</tt> (but it is legal to #THROW an exception).
 *
 * @post
 *   - A #FINALLY block will be executed after the #TRY block and any #CATCH
 *     block that might be executed, no matter whether the #TRY block
 *     *succeeds*, *recovers* or *fails*.
 *
 * @see #CATCH
 * @see #FINALLY
 * @see #RETRY
 * @see #e4c_is_uncaught
 */
#define TRY                                                                 \
                                                                            \
  EXCEPTIONS4C_START_BLOCK(false)                                           \
  if (e4c_try(EXCEPTIONS4C_DEBUG))

/**
 * Introduces a block of code capable of handling a specific type of exceptions
 *
 * @param type the type of exceptions to be handled.
 *
 * #CATCH blocks are optional code blocks that **must** be preceded by #TRY,
 * #WITH... #USE or #USING blocks. Several #CATCH blocks can be placed
 * next to one another.
 *
 * When an exception is thrown, the system looks for a #CATCH block to handle
 * it. The first capable block (in order of appearance) will be executed. The
 * exception is said to be *caught*.
 *
 * The caught exception can be accessed through the function #e4c_get_exception.
 *
 * ```c
 * TRY {
 *   ...
 * } CATCH(MyExceptionType) {
 *   const e4c_exception * exception = e4c_get_exception();
 *   printf("Error: %s", exception->message);
 * }
 * ```
 *
 * After the #CATCH block completes, the #FINALLY block (if any) is executed.
 * Then the program continues by the next line following the set of #TRY...
 * #CATCH... #FINALLY blocks.
 *
 * However, if an exception is thrown in a #CATCH block, then the #FINALLY
 * block will be executed right away and the system will look for an outter
 * #CATCH block to handle it.
 *
 * Only one of all the #CATCH blocks will be executed for each #TRY block,
 * even though the executed #CATCH block throws another exception. The only
 * possible way to execute more than one #CATCH block would be by [retrying](#RETRY)
 * the entire #TRY block.
 *
 * @pre
 *   - A #CATCH block **must** be preceded by one of these blocks:
 *     - A #TRY block
 *     - A #WITH... #USE block
 *     - A #USING block
 *     - Another #CATCH block
 *   - A #CATCH block **must not** be exited through any of: <tt>goto</tt>, <tt>break</tt>,
 *     <tt>continue</tt>, or <tt>return</tt> (but it is legal to #THROW an exception).
 *
 * @see #TRY
 * @see #CATCH_ALL
 * @see #e4c_exception_type
 * @see #e4c_get_exception
 * @see #e4c_exception
 */
#define CATCH(type)                                                         \
                                                                            \
  else if (e4c_catch(&type, EXCEPTIONS4C_DEBUG))

/**
 * Introduces a block of code capable of handling any exception
 *
 * #CATCH_ALL blocks are optional code blocks that **must** be preceded by #TRY,
 * #WITH... #USE or #USING blocks. No other #CATCH_ALL or #CATCH blocks can be placed
 * next to a #CATCH_ALL block.
 *
 * @see #CATCH
 */
#define CATCH_ALL                                                           \
                                                                            \
  else if (e4c_catch(NULL, EXCEPTIONS4C_DEBUG))

/**
 * Introduces a block of code responsible for cleaning up the previous
 * exception-aware block
 *
 * #FINALLY blocks are optional code blocks that **must** be preceded by
 * #TRY, #WITH... #USE or #USING blocks. It is allowed to place, at
 * most, one #FINALLY block for each one of these.
 *
 * The #FINALLY block can determine the completeness of the *exception-aware*
 * block through the function #e4c_is_uncaught. The thrown exception (if any)
 * can also be accessed through the function #e4c_get_exception.
 *
 * ```c
 * TRY {
 *   ...
 * } FINALLY {
 *   if (e4c_get_exception() != NULL) {
 *     if (e4c_is_uncaught()) {
 *       // The TRY block failed with an uncaught exception
 *     } else {
 *       // The TRY block failed but the exception was caught
 *     }
 *   } else {
 *     // The TRY block completed successfully
 *   }
 * }
 * ```
 *
 * The #FINALLY block will be executed only **once**. The only possible way to
 * execute it again would be by [retrying](#RETRY) the entire #TRY block.
 *
 * @pre
 *   - A #FINALLY block MUST be preceded by a #TRY, #WITH... #USE, #USING or #CATCH block.
 *   - A #FINALLY block MUST NOT be exited through any of: <tt>goto</tt>, <tt>break</tt>, <tt>continue</tt>, or <tt>return</tt> (but it is legal to #THROW an exception).
 *
 * @see #e4c_exception
 * @see #e4c_get_exception
 * @see #e4c_is_uncaught
 */
#define FINALLY                                                             \
                                                                            \
  else if (e4c_finally(EXCEPTIONS4C_DEBUG))

/**
 * Signals an exceptional situation represented by an exception object
 *
 * @param type the type of exception to be thrown.
 * @param format the error message.
 * @param ... an optional list of arguments that will be formatted according to <tt>format</tt>.
 *
 * Creates a new exception of the specified type of exception and then throws it.
 *
 * If <strong>format</strong> is <tt>NULL</tt>, then the default message for that type of exception will
 * be used. Otherwise, it MAY contain printf-like format specifications that
 * determine how the variadic arguments will be interpreted.
 *
 * When an exception is thrown, the exception handling framework looks for the
 * appropriate #CATCH block that can handle the exception. The system unwinds
 * the call chain of the program and executes the #FINALLY blocks it finds.
 *
 * When no #CATCH block is able to handle an exception, the system eventually
 * gets to the main function of the program. This situation is called an
 * **uncaught exception**.
 *
 * @post
 *   - Control does not return to the #THROW point.
 *
 * @see #e4c_exception_type
 * @see #e4c_exception
 * @see #e4c_get_exception
 */
#define THROW(type, format, ...)                                            \
                                                                            \
  EXCEPTIONS4C_LONG_JUMP(                                                   \
    e4c_throw(                                                              \
      &type,                                                                \
      #type,                                                                \
      EXCEPTIONS4C_DEBUG,                                                   \
      (format)                                                              \
      __VA_OPT__(,) __VA_ARGS__                                             \
    )                                                                       \
  )

/**
 * Repeats the previous #TRY (or #USE) block entirely
 *
 * @param max_attempts the maximum number of attempts to retry.
 * @param type the type of exception to be thrown.
 * @param format the error message.
 * @param ... an optional list of arguments that will be formatted according to <tt>format</tt>.
 *
 * This macro repeats the previous #TRY or #USE block, up to a specified
 * maximum number of attempts. If the block has already been *tried* at
 * least the specified number of times, then the supplied exception will
 * be thrown.
 *
 * It is intended to be used within #CATCH or #FINALLY blocks as
 * a quick way to fix an error condition and *try again*.
 *
 * ```c
 * const char * file_path = config_get_user_defined_file_path();
 *
 * TRY {
 *   config = read_config(file_path);
 * } CATCH(ConfigException) {
 *   file_path = config_get_default_file_path();
 *   RETRY(1, ConfigException, "Wrong defaults.");
 * }
 * ```
 *
 * @note
 * #RETRY MAY be used at a #FINALLY block.
 *
 * ```c
 * int dividend = 100;
 * int divisor = 0;
 * int result = 0;
 *
 * TRY {
 *   result = dividend / divisor;
 *   do_something(result);
 * } FINALLY {
 *   if (e4c_is_uncaught()) {
 *     divisor = 1;
 *     RETRY(1, NoMoreRetriesException, "Retry Error");
 *   }
 * }
 * ```
 *
 * @pre
 *   - A #RETRY statement MUST be placed in #CATCH or #FINALLY blocks only.
 * @post
 *   - Control does not return to the #RETRY point.
 *
 * @see #REACQUIRE
 * @see #TRY
 * @see #USE
 * @see #e4c_is_uncaught
 */
#define RETRY(max_attempts, type, format, ...)                              \
                                                                            \
  EXCEPTIONS4C_LONG_JUMP(                                                   \
    e4c_restart(                                                            \
      false,                                                                \
      max_attempts,                                                         \
      &type,                                                                \
      #type,                                                                \
      EXCEPTIONS4C_DEBUG,                                                   \
      (format)                                                              \
      __VA_OPT__(,) __VA_ARGS__                                             \
    )                                                                       \
  )

/**
 * Opens a block of code with automatic disposal of a resource
 *
 * @param resource the resource to be disposed
 * @param dispose the function (or macro) to dispose of the resource
 *
 * The combination of #WITH... #USE encapsulates the *Dispose Pattern*.
 * This pattern consists of two separate blocks and an implicit call
 * to a given function:
 *
 * - The #WITH block is responsible for the resource acquisition.
 * - The #USE block makes use of the resource.
 * - The <strong>dispose</strong> function (or macro) is responsible for the resource disposal.
 *
 * A #WITH block MUST be followed by one #USE block. In addition, the #USE
 * block may be followed by several #CATCH blocks and/or one #FINALLY block.
 *
 * The <strong>dispose</strong> function (or macro) will be automatically invoked right after
 * the #USE block *if, and only if*, the #WITH block completes. Otherwise,
 * neither <strong>dispose</strong> nor the #USE block will be executed.
 *
 * If an exception is thrown while using the resource, the possible #CATCH
 * or #FINALLY blocks (if any) will take place **after** the disposal of
 * the resource.
 *
 * When called, the disposal function will receive two arguments:
 *
 *   - The resource
 *   - A boolean flag indicating if the #USE block did not complete
 *
 * This way, different actions can be taken depending on the success or failure
 * of the block. For example, commiting or rolling back a *transaction*
 * resource.
 *
 * Legacy functions can be reused by defining macros. For example, a file
 * resource needs to be closed regardless of the errors occurred. Since the
 * function <strong>fclose</strong> only takes one parameter, we could define the next macro:
 *
 * ```c
 * #define CLOSE_FILE(file, failed) fclose(file)
 * ```
 *
 * Here is the typical usage of #WITH... #USE:
 *
 * ```c
 * const struct e4c_exception_type file_error = {NULL, "File error"};
 * const struct e4c_exception_type config_error = {NULL, "Config error"};
 * FILE * file;
 * char title[256] = "";
 * WITH(file, CLOSE_FILE) {
 *   file = fopen("title.txt", "r");
 *   if (file == NULL) {
 *     THROW(file_error, "Could not open title.txt");
 *   }
 * } USE {
 *   size_t read = fread(title, sizeof(title), 1, file);
 *   if (read < 1) {
 *     THROW(config_error, "Could not read title");
 *   }
 * } CATCH(file_error) {
 *   ...
 * } CATCH(config_error) {
 *   ...
 * } FINALLY {
 *   ...
 * }
 * ```
 *
 * @pre
 *   - A #WITH block MUST NOT be exited through any of: <tt>goto</tt>, <tt>break</tt>,
 *     <tt>continue</tt>, or <tt>return</tt> (but it is legal to #THROW an exception).
 *   - A #WITH block MUST always be followed by one #USE block.
 *
 * @see #USE
 * @see #USING
 */
#define WITH(resource, dispose)                                             \
                                                                            \
  EXCEPTIONS4C_START_BLOCK(true)                                            \
  if (e4c_dispose(EXCEPTIONS4C_DEBUG)) {                                    \
    (void) dispose((resource), e4c_is_uncaught());                          \
  } else if (e4c_acquire(EXCEPTIONS4C_DEBUG)) {

/**
 * Closes a block of code with automatic disposal of a resource
 *
 * A #USE block **must** always be preceded by a #WITH block. These two
 * keywords are designed so the compiler will complain about *dangling*
 * #WITH... #USE blocks.
 *
 * A code block introduced by the #USE keyword will only be executed *if, and
 * only if*, the acquisition of the resource *completes* without exceptions.
 *
 * The disposal function will be executed, either if the #USE block completes
 * or not.
 *
 * @pre
 *   - A #USE block MUST be preceded by a #WITH block.
 *   - A #USE block MUST NOT be exited through any of: <tt>goto</tt>, <tt>break</tt>,
 *     <tt>continue</tt>, or <tt>return</tt> (but it is legal to #THROW an exception).
 *
 * @see #WITH
 */
#define USE                                                                 \
                                                                            \
  } else if (e4c_try(EXCEPTIONS4C_DEBUG))

/**
 * Introduces a block of code with automatic acquisition and disposal of a
 * resource
 *
 * @param resource the resource to be acquired, used and then disposed.
 * @param dispose the function (or macro) to dispose of the resource.
 * @param acquire the function (or macro) to acquire the resource.
 * @param ... an optional list of arguments to be passed to <tt>acquire</tt>.
 *
 * The specified resource will be *acquired*, *used* and then *disposed*. The
 * automatic acquisition and disposal is achieved by calling the supplied
 * functions (or macros) <strong>acquire</strong> and <strong>dispose</strong>:
 *
 *   - <tt>typeof(resource) acquire(...)</tt>
 *   - <tt>void dispose(typeof(resource) resource, bool is_uncaught)</tt>
 *
 * The semantics of the automatic acquisition and disposal are the same as for
 * blocks introduced by #WITH... #USE. For example, a #USING block can also
 * precede #CATCH and #FINALLY blocks.
 *
 * @pre
 *   - A #USING block MUST NOT be exited through any of: <tt>goto</tt>, <tt>break</tt>,
 *     <tt>continue</tt>, or <tt>return</tt> (but it is legal to #THROW an exception).
 *
 * @see #WITH
 */
#define USING(resource, dispose, acquire, ...)                              \
                                                                            \
  WITH((resource), dispose) {                                               \
    (resource) = (void) &(resource), acquire(__VA_ARGS__);                  \
  } USE

/**
 * Repeats the previous #WITH block entirely
 *
 * @param max_attempts the maximum number of attempts to reacquire
 * @param type the type of exception to be thrown when <tt>max_attempts</tt> is exceeded.
 * @param format The error message.
 * @param ... an optional list of arguments that will be formatted according to <tt>format</tt>.
 *
 * This macro repeats the previous #WITH block, up to a specified maximum
 * number of attempts. If the acquisition completes, then the #USE block
 * will be executed. Otherwise, the supplied exception type will be thrown.
 *
 * @remark
 * #REACQUIRE SHOULD be used within #CATCH or #FINALLY blocks, next to a #WITH... #USE or #USING block, when
 * a resource cannot be acquired, as a quick way to fix an error condition and try to acquire the resource again.
 *
 * ```c
 * image_type * image;
 * const char * image_path = image_get_user_avatar();
 *
 * WITH(image, e4c_image_dispose) {
 *   image = e4c_image_acquire(image_path);
 * } USE {
 *   image_show(image);
 * } CATCH(ImageNotFoundException) {
 *   image_path = image_get_default_avatar();
 *   REACQUIRE(1);
 * }
 * ```
 *
 * Once the resource has been acquired, the #USE block can also be repeated *alone* through the keyword #RETRY:
 *
 * ```c
 * image_type * image;
 * const char * image_path = image_get_user_avatar();
 * display_type * display = display_get_user_screen();
 * 
 * WITH(image, e4c_image_dispose) {
 *   image = e4c_image_acquire(image_path);
 * } USE {
 *   image_show(image, display);
 * } CATCH(ImageNotFoundException) {
 *   image_path = image_get_default_avatar();
 *   REACQUIRE(1);
 * } CATCH(DisplayException) {
 *   display = display_get_default_screen();
 *   RETRY(1);
 * }
 * ```
 *
 * @pre
 *   - The #REACQUIRE keyword **must** be used from a #CATCH or #FINALLY
 *     block, preceded by #WITH... #USE or #USING blocks.
 * @post
 *   - Control does not return to the #REACQUIRE point.
 *
 * @see #RETRY
 * @see #WITH
 * @see #USE
 * @see #e4c_is_uncaught
 */
#define REACQUIRE(max_attempts, type, format, ...)                          \
                                                                            \
  EXCEPTIONS4C_LONG_JUMP(                                                   \
    e4c_restart(                                                            \
      true,                                                                 \
      max_attempts,                                                         \
      &type,                                                                \
      #type,                                                                \
      EXCEPTIONS4C_DEBUG,                                                   \
      (format)                                                              \
      __VA_OPT__(,) __VA_ARGS__                                             \
    )                                                                       \
  )

/**
 * @internal Starts a new exception block.
 *
 * @param should_acquire if <tt>true</tt>, the exception block will start #ACQUIRING a resource.
 */
#define EXCEPTIONS4C_START_BLOCK(should_acquire)                            \
                                                                            \
  for (                                                                     \
    EXCEPTIONS4C_SET_JUMP(e4c_start(should_acquire, EXCEPTIONS4C_DEBUG));   \
    e4c_next(EXCEPTIONS4C_DEBUG) || (                                       \
      e4c_is_uncaught() && (EXCEPTIONS4C_LONG_JUMP(e4c_get_env()), true)    \
    );                                                                      \
  )

#ifndef HAVE_SIGSETJMP

/**
 * @internal Saves the current execution context.
 *
 * @param env the variable to store the current [execution context](#e4c_env).
 */
#define EXCEPTIONS4C_SET_JUMP(env) setjmp(*(env))

/**
 * @internal Loads the supplied execution context.
 *
 * @param env the [execution context](#e4c_env) to load.
 */
#define EXCEPTIONS4C_LONG_JUMP(env) longjmp(*(env), 1)

/** @internal Stores information to restore a calling environment. */
typedef jmp_buf e4c_env;

#else

/**
 * @internal Saves the current execution context.
 *
 * @param env the variable to store the current [execution context](#e4c_env).
 */
#define EXCEPTIONS4C_SET_JUMP(env) sigsetjmp(*(env), 1)

/**
 * @internal Loads the supplied execution context.
 *
 * @param env the [execution context](#e4c_env) to load.
 */
#define EXCEPTIONS4C_LONG_JUMP(env) siglongjmp(*(env), 1)

/** @internal Stores information to restore a calling environment. */
typedef sigjmp_buf e4c_env;

#endif

#ifndef NDEBUG

/** @internal Captures debug information about the running program. */
#define EXCEPTIONS4C_DEBUG __FILE__, __LINE__, __func__

#else

/** @internal Discards debug information about the running program. */
#define EXCEPTIONS4C_DEBUG NULL, 0, NULL

#endif

/**
 * Represents a specific type of errors in the system.
 *
 * Exception types indicate conditions that a program might want to
 * #THROW and/or #CATCH.
 *
 * - They MAY have a <strong>supertype</strong> to organize them hierarchically.
 *   This is useful when you #CATCH them.
 * - They SHOULD have a <strong>default_message</strong> describing the problem they represent.
 *   This is useful when you #THROW them.
 * - They SHOULD be defined as <tt>const</tt>.
 *
 * ```c
 * const struct e4c_exception_type EXCEPTION1 = {NULL, "Exception one"};
 * const struct e4c_exception_type EXCEPTION2 = {&EXCEPTION, "Exception two"};
 * ```
 *
 * @see #THROW
 * @see #CATCH
 */
struct e4c_exception_type {

    /** The possibly-null supertype of this type */
    const struct e4c_exception_type * supertype;

    /** The default message for new exceptions of this type */
    const char * default_message;
};

/**
 * Represents a problematic situation during program execution.
 *
 * Exceptions are a means of breaking out of the normal flow of control of a
 * code block in order to handle errors or other exceptional conditions. An
 * exception should be [thrown](#THROW) at the point where the error is
 * detected; it may be [handled](#CATCH) by the surrounding code block or by
 * any code block that directly or indirectly invoked the code block where the
 * error occurred.
 *
 * Exceptions provide information regarding the exceptional situation, such as:
 *
 * @remark
 * You MAY associate user-defined <strong>data</strong> to exceptions by configuring [initialize_exception](#e4c_context.initialize_exception) in the [exception context](e4c_context).
 *
 * @see #THROW
 * @see #CATCH
 * @see #e4c_exception_type
 * @see #e4c_get_exception
 * @see #e4c_context
 */
struct e4c_exception {

    /** A non-null pointer to the type of this exception. */
    const struct e4c_exception_type * type;

    /** The name of this exception's type. */
    const char * name;

    /** A text message describing the specific problem. */
    char message[256];

    /** The name of the source code file that threw this exception, or <tt>NULL</tt> if <tt>NDEBUG</tt> is defined. */
    const char * file;

    /** The number of line that threw this exception, or zero if <tt>NDEBUG</tt> is defined. */
    int line;

    /** The name of the function that threw this exception, or <tt>NULL</tt> if <tt>NDEBUG</tt> is defined. */
    const char * function;

    /** The value of [errno](https://devdocs.io/c/error/errno) at the time this exception was thrown. */
    int error_number;

    /** A possibly-null pointer to the current exception when this exception was thrown. */
    struct e4c_exception * cause;

    /** A possibly-null pointer to custom data associated to this exception. */
    void * data;

    /* @internal Number of times this exception is referenced. */
    int _ref_count;
};

/**
 * Contains the configuration and the current status of exceptions.
 *
 * This structure allows you to configure the way the exception system behaves:
 *
 * - Set <strong>uncaught_handler</strong> to a function that will be executed when the program abruptly terminates due to an uncaught exception.
 * - Set <strong>initialize_exception</strong> to a function that will be executed whenever an exception is thrown. This function MAY create and assign custom data to the exception.
 * - Set <strong>finalize_exception</strong> to a function that will be executed whenever an exception is deleted. This function MAY delete custom data previously created.
 *
 * @see #e4c_get_context
 * @see #e4c_set_context_supplier
 */
struct e4c_context {

    /**
     * @internal The current exception block of the running program.
     */
    void * _innermost_block;

    /** The function to be executed in the event of an uncaught exception */
    void (*uncaught_handler)(const struct e4c_exception * exception);

    /** The function to be executed whenever a new exception is thrown */
    void (*initialize_exception)(struct e4c_exception * exception);

    /** The function to be executed whenever an exception is destroyed */
    void (*finalize_exception)(const struct e4c_exception * exception);
};

/**
 * Sets the exception context supplier.
 *
 * @param supplier a function that supplies the current exception context.
 *
 * The library relies on this [context](#e4c_context) to handle the current
 * status of exceptions. If no supplier is provided, a default one will be used.
 *
 * @remark
 * Since the default exception context does not support concurrency, this
 * mechanism can be useful to provide a concurrent version. For example, a
 * context supplier could return different instances, depending on which
 * thread is active. In that case, the supplier MUST be responsible for
 * the creation and deletion of those instances.
 *
 * @see #e4c_context
 */
void e4c_set_context_supplier(struct e4c_context * (*supplier)(void));

/**
 * Retrieves the current exception context.
 *
 * @return the current exception context.
 *
 * @remark
 * This function MAY be used to configure the current exception context.
 * For example, you can set a custom handler that will be executed when
 * the program abruptly terminates due to an uncaught exception.
 *
 * @see #e4c_context
 * @see #e4c_set_context_supplier
 */
struct e4c_context * e4c_get_context(void);

/**
 * Retrieves the exception that was thrown in the current exception block.
 *
 * @return a possibly-null pointer to the exception that was thrown in the innermost exception block.
 *
 * This function returns a pointer to the exception that was thrown in the
 * surrounding *exception-aware* block, if any; otherwise <tt>NULL</tt>.
 *
 * The thrown exception can be obtained any time, provided that the exception
 * context has begun at the time of the function call. However, it is sensible
 * to call this function only during the execution of #FINALLY or #CATCH
 * blocks.
 *
 * Moreover, a pointer to the thrown exception obtained *inside* a #FINALLY
 * or #CATCH block **must not** be used *outside* these blocks.
 *
 * The exception system objects are dynamically allocated and deallocated, as
 * the program enters or exits #TRY... #CATCH... #FINALLY blocks. While
 * it would be legal to *copy* the thrown exception and access to its <strong>name</strong>
 * and <strong>message</strong> outside these blocks, care should be taken in order not to
 * dereference the <strong>cause</strong> of the exception, unless it is a **deep copy**
 * (as opposed to a **shallow copy**).
 *
 * @see #e4c_exception
 * @see #THROW
 * @see #CATCH
 * @see #FINALLY
 */
const struct e4c_exception * e4c_get_exception(void);

/**
 * Returns whether the current exception block has an uncaught exception.
 *
 * @return <tt>true</tt> if the innermost exception block has an uncaught exception; <tt>false</tt> otherwise.
 *
 * @remark
 * This function MAY be called during the execution of #FINALLY blocks to
 * determine if the exception block completed successfully or not.
 *
 * @see #FINALLY
 * @see #e4c_get_exception
 */
bool e4c_is_uncaught(void);

/**
 * @internal Starts a new exception block.
 *
 * @param should_acquire if <tt>true</tt>, the exception block will start in the #ACQUIRING stage; otherwise it will start in the #TRYING stage.
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @return the execution context of the new exception block.
 *
 * @warning This function SHOULD be called only via #EXCEPTIONS4C_START_BLOCK.
 */
e4c_env * e4c_start(bool should_acquire, const char * file, int line, const char * function);

/**
 * @internal Iterates through the [different stages](#block_stage) of the current exception block.
 *
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @return <tt>true</tt> if the current exception block has not completed yet; <tt>false</tt> otherwise.
 *
 * @warning This function SHOULD be called only via #EXCEPTIONS4C_START_BLOCK.
 */
bool e4c_next(const char * file, int line, const char * function);

/**
 * @internal Retrieves the execution context of the current exception block.
 *
 * @return the execution context of the current exception block.
 *
 * @warning This function SHOULD be called only via #EXCEPTIONS4C_START_BLOCK.
 */
e4c_env * e4c_get_env(void);

/**
 * @internal Checks if the current exception block is in the #ACQUIRING stage.
 *
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @return <tt>true</tt> if the current exception block is in the #ACQUIRING stage; <tt>false</tt> otherwise.
 *
 * @warning This function SHOULD be called only via #WITH.
 */
bool e4c_acquire(const char * file, int line, const char * function);

/**
 * @internal Checks if the current exception block is in the #TRYING stage.
 *
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @return <tt>true</tt> if the current exception block is in the #TRYING stage; <tt>false</tt> otherwise.
 *
 * @warning This function SHOULD be called only via #TRY.
 */
bool e4c_try(const char * file, int line, const char * function);

/**
 * @internal Checks if the current exception block is in the #DISPOSING stage.
 *
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @return <tt>true</tt> if the current exception block is in the #DISPOSING stage; <tt>false</tt> otherwise.
 *
 * @warning This function SHOULD be called only via #WITH.
 */
bool e4c_dispose(const char * file, int line, const char * function);

/**
 * @internal Checks if the current exception can be handled.
 *
 * @param type the type of exceptions to be handled.
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @return <tt>true</tt> if:
 *   - the current exception block is in the #CATCHING stage, AND
 *   - the supplied <tt>type</tt> is either <tt>NULL</tt> or a supertype of the thrown exception.
 *
 * @warning This function SHOULD be called only via #CATCH.
 */
bool e4c_catch(const struct e4c_exception_type * type, const char * file, int line, const char * function);

/**
 * @internal Checks if the current exception block is in the #FINALIZING stage.
 *
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @return <tt>true</tt> if the current exception block is in the #FINALIZING stage; <tt>false</tt> otherwise.
 *
 * @warning This function SHOULD be called only via #FINALLY.
 */
bool e4c_finally(const char * file, int line, const char * function);

/**
 * @internal Throws a new exception.
 *
 * @param type the type of exception to be thrown.
 * @param name the name of the exception type.
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @param format the error message.
 * @param ... an optional list of arguments that will be formatted according to <tt>format</tt>.
 * @return the execution context of the current exception block.
 *
 * @warning This function SHOULD be called only via #THROW.
 */
e4c_env * e4c_throw(const struct e4c_exception_type * type, const char * name, const char * file, int line, const char * function, const char * format, ...);

/**
 * @internal Restarts an exception block.
 *
 * @param should_reacquire if <tt>true</tt>, the exception block will restart in the #ACQUIRING stage; otherwise it will start in the #TRYING stage.
 * @param max_attempts
 * @param type the type of exception to be thrown.
 * @param name the name of the exception type.
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @param format the error message.
 * @param ... an optional list of arguments that will be formatted according to <tt>format</tt>.
 * @return the execution context of the current exception block.
 *
 * @warning This function SHOULD be called only via #RETRY or #REACQUIRE.
 */
e4c_env * e4c_restart(bool should_reacquire, int max_attempts, const struct e4c_exception_type * type, const char * name, const char * file, int line, const char * function, const char * format, ...);

#endif
