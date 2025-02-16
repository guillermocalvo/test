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
 * To use it in your project, include the header file in your
 * source code files.
 *
 * ```c
 * #include <exceptions4c.h>
 * ```
 *
 * And then link your program against the library code.
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

# ifdef __cplusplus
extern "C" {
# endif

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
  EXCEPTIONS4C_START_BLOCK(false)                                           \
  if (e4c_try(EXCEPTIONS4C_DEBUG))

/**
 * Introduces a block of code that handles exceptions thrown by a
 * preceding #TRY block.
 *
 * @param exception_type the type of exception to catch.
 *
 * One or more #CATCH blocks can follow a #TRY block. Each #CATCH block
 * MUST specify the type of exception it handles.
 *
 * @see TRY
 * @see CATCH_ALL
 *
 */
#define CATCH(exception_type)                                               \
                                                                            \
  else if (e4c_catch(&exception_type, EXCEPTIONS4C_DEBUG))

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
  else if (e4c_catch(NULL, EXCEPTIONS4C_DEBUG))

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
  else if (e4c_finally(EXCEPTIONS4C_DEBUG))

/**
 * Throws an exception, interrupting the normal flow of execution.
 *
 * @param exception_type the type of the exception to throw.
 * @param format the error message.
 * @param ... a list of arguments that will be formatted according to
 *   <tt>format</tt>.
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
 *
 * @note
 * The error message will be formatted just as it would with
 * <tt>printf</tt>. If no message is specified, then the default message
 * for the exception type will be used.
 */
#define THROW(exception_type, format, ...)                                  \
                                                                            \
  EXCEPTIONS4C_LONG_JUMP(                                                   \
    e4c_throw(                                                              \
      &exception_type,                                                      \
      #exception_type,                                                      \
      EXCEPTIONS4C_DEBUG,                                                   \
      (format)                                                              \
      __VA_OPT__(,) __VA_ARGS__                                             \
    )                                                                       \
  )

/**
 * Repeats the previous #TRY or #USE block entirely
 *
 * @param max_attempts the maximum number of attempts to retry.
 * @param type the type of exception to throw.
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
 * @see REACQUIRE
 * @see TRY
 * @see USE
 * @see e4c_is_uncaught
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
 * Introduces a block of code with automatic acquisition and disposal of a
 * resource
 *
 * @param resource the thing to acquire, use and then dispose of.
 * @param predicate the condition that determines if the resource can be used.
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
 * @see WITH
 */
#define USING(resource, predicate, dispose, acquire, ...)                   \
                                                                            \
  WITH((resource), dispose) {                                               \
    (resource) = acquire(__VA_ARGS__);                                      \
  } USE (predicate)

/**
 * Opens a block of code with automatic disposal of a resource
 *
 * @param resource the thing to dispose of.
 * @param dispose the function (or macro) to dispose of the <strong>resource</strong>.
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
 * @see USE
 * @see USING
 */
#define WITH(resource, dispose)                                             \
                                                                            \
  EXCEPTIONS4C_START_BLOCK(true)                                            \
  if (e4c_dispose(EXCEPTIONS4C_DEBUG)) {                                    \
    (void) dispose(resource);                                               \
  } else if (e4c_acquire(EXCEPTIONS4C_DEBUG)) {

/**
 * Closes a block of code with automatic disposal of a resource
 *
 * @param predicate the condition that determines if the resource can be used.
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
 * @see WITH
 */
#define USE(predicate)                                                      \
                                                                            \
  } else if (e4c_try(EXCEPTIONS4C_DEBUG) && (predicate))

/**
 * Repeats the previous #WITH block entirely
 *
 * @param max_attempts the maximum number of attempts to reacquire
 * @param type the type of exception throw when <tt>max_attempts</tt> is exceeded.
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
 * @see RETRY
 * @see WITH
 * @see USE
 * @see e4c_is_uncaught
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
 * Represents a category of problematic situations in a program.
 *
 * Defines a kind of error or exceptional condition that a program might
 * want to #THROW and #CATCH. It serves as a way to group related issues
 * that share common characteristics.
 *
 * Exception types SHOULD be defined as <tt>const</tt>.
 *
 * ```c
 * const e4c_exception_type IO_ERROR = "I/O Error";
 * ```
 *
 * @see THROW
 * @see CATCH
 */
struct e4c_exception_type {

    /** The possibly-null supertype of this type */
    const struct e4c_exception_type * supertype;

    /** The default message for new exceptions of this type */
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
 * When an exception is caught, #e4c_get_exception can be used to
 * retrieve the exception currently being handled. This allows for
 * inspection and further handling of the error based on both its type
 * and the detailed context of the situation.
 */
struct e4c_exception {

    /** The general nature of the error. */
    const struct e4c_exception_type * type;

    /** The name of the exception type. */
    const char * name;

    /** A text message describing the specific problem. */
    char message[256];

    /** The name of the source file that threw this exception, or <tt>NULL</tt> if <tt>NDEBUG</tt> is defined. */
    const char * file;

    /** The line number in the source file that threw this exception, or zero if <tt>NDEBUG</tt> is defined. */
    int line;

    /** The name of the function that threw this exception, or <tt>NULL</tt> if <tt>NDEBUG</tt> is defined. */
    const char * function;

    /** The value of [errno](https://devdocs.io/c/error/errno) at the time this exception was thrown. */
    int error_number;

    /** A possibly-null pointer to the current exception when this exception was thrown. */
    struct e4c_exception * cause;

    /** A possibly-null pointer to custom data associated to this exception. */
    void * data;
};

/**
 * Contains the configuration and the current status of exceptions.
 *
 * This structure allows you to configure the way the exception system behaves:
 *
 * - Set <strong>uncaught_handler</strong> to a function that will be executed when an exception reaches the top level of the program.
 * - Set <strong>termination_handler</strong> to a function that will be executed when the program abruptly terminates due to an uncaught exception.
 * - Set <strong>initialize_exception</strong> to a function that will be executed whenever an exception is thrown. This function MAY create and assign custom data to the exception.
 * - Set <strong>finalize_exception</strong> to a function that will be executed whenever an exception is deleted. This function MAY delete custom data previously created.
 *
 * @see e4c_get_context
 * @see e4c_set_context_supplier
 */
struct e4c_context {

    /**
     * @internal The current exception block of the running program.
     */
    void * _innermost_block;

    /** The function to execute in the event of an uncaught exception */
    void (*uncaught_handler)(const struct e4c_exception * exception);

    /** The function to execute in the event of program termination. */
    void (*termination_handler)();

    /** The function to execute whenever a new exception is thrown */
    void (*initialize_exception)(struct e4c_exception * exception);

    /** The function to execute whenever an exception is destroyed */
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
 * @see e4c_context
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
 * @see e4c_context
 * @see e4c_set_context_supplier
 */
struct e4c_context * e4c_get_context(void);

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
 * @see e4c_exception
 * @see THROW
 * @see CATCH
 * @see FINALLY
 * @see e4c_is_uncaught
 */
const struct e4c_exception * e4c_get_exception(void);

/**
 * Determines whether the current exception (if any) hasn't been handled
 * yet by any #CATCH or #CATCH_ALL block.
 *
 * @return whether the thrown exception was not caught by any #CATCH or
 *   #CATCH_ALL block.
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
 * @see e4c_get_exception
 */
bool e4c_is_uncaught(void);

/**
 * @internal
 * @brief Starts a new exception block.
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
 * @internal
 * @brief Iterates through the different [stages](#block_stage) of the current exception block.
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
 * @internal
 * @brief Retrieves the execution context of the current exception block.
 *
 * @return the execution context of the current exception block.
 *
 * @warning This function SHOULD be called only via #EXCEPTIONS4C_START_BLOCK.
 */
e4c_env * e4c_get_env(void);

/**
 * @internal
 * @brief Checks if the current exception block is in the #ACQUIRING stage.
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
 * @internal
 * @brief Checks if the current exception block is in the #TRYING stage.
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
 * @internal
 * @brief Checks if the current exception block is in the #DISPOSING stage.
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
 * @internal
 * @brief Checks if the current exception can be handled.
 *
 * @param type the type of exceptions to handle.
 * @param file the name of the source code file that is calling this function.
 * @param line the number of line that is calling this function.
 * @param function the name of the function that is calling this function.
 * @return <tt>true</tt> if:
 *   - the current exception block is in the #CATCHING stage, AND
 *   - the supplied <tt>type</tt> is either <tt>NULL</tt> or a supertype of the thrown exception.
 *   <tt>false</tt> otherwise.
 *
 * @warning This function SHOULD be called only via #CATCH.
 */
bool e4c_catch(const struct e4c_exception_type * type, const char * file, int line, const char * function);

/**
 * @internal
 * @brief Checks if the current exception block is in the #FINALIZING stage.
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
 * @internal
 * @brief Throws a new exception.
 *
 * @param type the type of exception to throw.
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
 * @internal
 * @brief Restarts an exception block.
 *
 * @param should_reacquire if <tt>true</tt>, the exception block will restart in the #ACQUIRING stage; otherwise it will start in the #TRYING stage.
 * @param max_attempts
 * @param type the type of exception to throw.
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

#ifdef __cplusplus
}
#endif

#endif
