/**
 *
 * @file        exceptions4c.h
 *
 * exceptions4c header file
 *
 * @version     4.0
 * @author      Copyright (c) 2016 Guillermo Calvo
 *
 * @section e4c_h exceptions4c header file
 *
 * This header file needs to be included in order to be able to use any of the
 * exception handling system keywords:
 *
 *   - #TRY
 *   - #CATCH
 *   - #FINALLY
 *   - #THROW
 *   - #WITH
 *   - #USING
 *
 * @section license License
 *
 * > This is free software: you can redistribute it and/or modify it under the
 * > terms of the **GNU Lesser General Public License** as published by the
 * > *Free Software Foundation*, either version 3 of the License, or (at your
 * > option) any later version.
 * >
 * > This software is distributed in the hope that it will be useful, but
 * > **WITHOUT ANY WARRANTY**; without even the implied warranty of
 * > **MERCHANTABILITY** or **FITNESS FOR A PARTICULAR PURPOSE**. See the
 * > [GNU Lesser General Public License](http://www.gnu.org/licenses/lgpl.html)
 * > for more details.
 * >
 * > You should have received a copy of the GNU Lesser General Public License
 * > along with this software. If not, see <http://www.gnu.org/licenses/>.
 *
 */


# ifndef EXCEPTIONS4C_VERSION

/**
 * Returns the major version number of this library.
 */
# define EXCEPTIONS4C_VERSION 4

#include <stdlib.h>
#include <setjmp.h>

#ifndef __bool_true_false_are_defined
#include <stdbool.h>
#endif

/**
 * @name Exception handling keywords
 *
 * This set of keywords express the semantics of exception handling.
 *
 * @{
 */

/**
 * Introduces a block of code aware of exceptions
 *
 * A #TRY statement executes a block of code. If an exception is thrown and
 * there is a #CATCH block that can handle it, then control will be
 * transferred to it. If there is a #FINALLY block, then it will be executed,
 * no matter whether the #TRY block completes normally or abruptly, and no
 * matter whether a #CATCH block is first given control.
 *
 * The block of code immediately after the keyword #TRY is called **the #TRY
 * block** of the #TRY statement. The block of code immediately after the
 * keyword #FINALLY is called **the #FINALLY block** of the #TRY statement.
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
 *   - A #TRY block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return` (but it is legal to #THROW an exception).
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
  EXCEPTIONS4C_START_BLOCK(false)                                           \
  if (e4c_try(EXCEPTIONS4C_DEBUG))

/**
 * Introduces a block of code capable of handling a specific type of exceptions
 *
 * @param   type
 *          The type of exceptions to be handled
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
 *   - A #CATCH block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return` (but it is legal to #THROW an exception).
 *
 * @see #TRY
 * @see #CATCH_ALL
 * @see #e4c_exception_type
 * @see #e4c_get_exception
 * @see #e4c_exception
 */
#define CATCH(type)                                                         \
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
 *   - A #FINALLY block **must** be preceded by a #TRY, `with`... `use`,
 *     `using` or #CATCH block.
 *   - A #FINALLY block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return` (but it is legal to #THROW an exception).
 *
 * @see #e4c_exception
 * @see #e4c_get_exception
 * @see #e4c_is_uncaught
 */
#define FINALLY                                                             \
  else if (e4c_finally(EXCEPTIONS4C_DEBUG))

/**
 * Signals an exceptional situation represented by an exception object
 *
 * @param   type
 *          The type of exception to be thrown
 * @param   format
 *          The detail message.
 * @param   ...
 *          The variadic arguments that will be formatted according to the
 *          format control
 *
 * Creates a new instance of the specified type of exception and then throws it.
 *
 * If `format` is `NULL`, then the default message for that type of exception will
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
 * @see #e4c_uncaught_handler
 * @see #e4c_get_exception
 */
#define THROW(type, format, ...)                                            \
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
 * @param   max_attempts
 *          The maximum number of attempts to retry
 * @param   type
 *          The type of exception to be thrown
 * @param   format
 *          The detail message.
 * @param   ...
 *          The variadic arguments that will be formatted according to the
 *          format control
 *
 * This macro repeats the previous #TRY or `use` block, up to a specified
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
 * #TRY MAY be used at a #FINALLY block.
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
 *   - The #RETRY keyword **must** be used from a #CATCH or #FINALLY block.
 * @post
 *   - Control does not return to the #RETRY point.
 *
 * @see #REACQUIRE
 * @see #TRY
 * @see #USE
 * @see #e4c_is_uncaught
 */
#define RETRY(max_attempts, type, format, ...)                              \
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

/** @} */

/**
 * @name Dispose pattern keywords
 *
 * This set of keywords express the semantics of automatic resource acquisition
 * and disposal.
 *
 * @{
 */

/**
 * Opens a block of code with automatic disposal of a resource
 *
 * @param   resource
 *          The resource to be disposed
 * @param   dispose
 *          The name of the disposal function (or macro)
 *
 * The combination of keywords #WITH... #USE encapsules the *Dispose
 * Pattern*. This pattern consists of two separate blocks and an implicit call
 * to a given function:
 *
 *   - the `with` block is responsible for the resource acquisition
 *   - the `use` block makes use of the resource
 *   - the disposal function will be called implicitly
 *
 * A `with` block **must** be followed by a `use` block. In addition, the `use`
 * block may be followed by several #CATCH blocks and/or one #FINALLY block.
 *
 * The `with` keyword guarantees that the disposal function will be called **if,
 * and only if**, the acquisition block is *completed* without any errors (i.e.
 * the acquisition block does not #THROW any exceptions).
 *
 * If the `with` block does not complete, neither the disposal function nor the
 * `use` block will be executed.
 *
 * The disposal function is called right after the `use` block. If an exception
 * is thrown, the #CATCH or #FINALLY blocks (if any) will take place **after**
 * the disposal of the resource.
 *
 * When called, the disposal function will receive two arguments:
 *
 *   - The resource
 *   - A boolean flag indicating if the `use` block *failed* (i. e. did not
 *     *complete*)
 *
 * This way, different actions can be taken depending on the success or failure
 * of the block. For example, commiting or rolling back a *transaction*
 * resource.
 *
 * Legacy functions can be reused by defining macros. For example, a file
 * resource needs to be closed regardless of the errors occurred. Since the
 * function `fclose` only takes one parameter, we could define the next macro:
 *
 * ```c
 * # define e4c_dispose_file(file , failed) fclose(file)
 * ```
 *
 * Here is the typical usage of #WITH... #USE:
 *
 * ```c
 * WITH(foo, e4c_dispose_foo) {
 *   foo = e4c_acquire_foo(foobar);
 *   validate_foo(foo, bar);
 *   ...
 * } USE {
 *   do_something(foo);
 *   ...
 * } CATCH(FooAcquisitionFailed) {
 *   recover1(foobar);
 *   ...
 * } CATCH(SomethingElseFailed) {
 *   recover2(foo);
 *   ...
 * } FINALLY {
 *   cleanup();
 *   ...
 * }
 * ```
 *
 * Nonetheless, *one-liners* fit nicely too:
 *
 * ```c
 * WITH(bar, e4c_dispose_bar) bar = e4c_acquire_bar(baz, 123); use foo(bar);
 * ```
 *
 * There is a way to lighten up even more this pattern by defining convenience
 * macros, customized for a specific kind of resources. For example,
 * `e4c_using_file` or `e4c_using_memory`.
 *
 * @pre
 *   - A `with` block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return` (but it is legal to `throw` an exception).
 *   - A `with` block **must** always be followed by a `use` block.
 *
 * @see #USE
 * @see #USING
 */
#define WITH(resource, dispose)                                             \
  EXCEPTIONS4C_START_BLOCK(true)                                            \
  if (e4c_dispose(EXCEPTIONS4C_DEBUG)) {                                    \
    dispose((resource), e4c_is_uncaught());                                 \
  } else if (e4c_acquire(EXCEPTIONS4C_DEBUG)) {

/**
 * Closes a block of code with automatic disposal of a resource
 *
 * A #USE block **must** always be preceded by a #WITH block. These two
 * keywords are designed so the compiler will complain about *dangling*
 * #WITH... #USE blocks.
 *
 * A code block introduced by the `use` keyword will only be executed *if, and
 * only if*, the acquisition of the resource *completes* without exceptions.
 *
 * The disposal function will be executed, either if the `use` block completes
 * or not.
 *
 * @pre
 *   - A #USE block **must** be preceded by a #WITH block.
 *   - A #USE block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return`  (but it is legal to #THROW an exception).
 *
 * @see #WITH
 */
#define USE                                                                 \
  } else if (e4c_try(EXCEPTIONS4C_DEBUG))

/**
 * Introduces a block of code with automatic acquisition and disposal of a
 * resource
 *
 * @param   type
 *          The type of the resource
 * @param   resource
 *          The resource to be acquired, used and then disposed
 * @param   args
 *          A list of arguments to be passed to the acquisition function
 *
 * The specified resource will be *acquired*, *used* and then *disposed*. The
 * automatic acquisition and disposal is achieved by calling the *implicit*
 * functions **e4c_acquire_<em>type</em>** and **e4c_dispose_<em>type</em>**:
 *
 *   - `TYPE e4c_acquire_TYPE(ARGS)`
 *   - `void e4c_dispose_TYPE(TYPE RESOURCE, bool failed)`
 *
 * These two symbols **must** exist, in the form of either *functions* or
 * *macros*.
 *
 * The semantics of the automatic acquisition and disposal are the same as for
 * blocks introduced by #WITH... #USE. For example, a #USING block can also
 * precede #CATCH and #FINALLY blocks.
 *
 * @pre
 *   - A `using` block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return`  (but it is legal to #THROW an exception).
 *
 * @see #WITH
 */
#define USING(type, resource, args)                                         \
  WITH((resource), e4c_dispose_##type) {                                    \
    (resource) = e4c_acquire_##type args;                                   \
  } USE

/**
 * Repeats the previous #WITH block entirely
 *
 * @param   max_attempts
 *          The maximum number of attempts to reacquire
 * @param   type
 *          The type of exception to be thrown
 * @param   format
 *          The detail message.
 * @param   ...
 *          The variadic arguments that will be formatted according to the
 *          format control
 *
 * This macro repeats the previous `with` block, up to a specified maximum
 * number of attempts. If the acquisition completes, then the `use` block
 * will be executed. Otherwise, the supplied exception will be thrown.
 *
 * It is intended to be used within #CATCH or #FINALLY blocks, next to a
 * #WITH... #USE or #USING block, when the resource acquisition fails,
 * as a quick way to fix an error condition and try to acquire the resource
 * again.
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
 * Once the resource has been acquired, the `use` block can also be repeated
 * *alone* through the keyword #RETRY:
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
 *     block, preceded by `with`... `use` or `using` blocks.
 * @post
 *   - Control does not return to the `reacquire` point.
 *
 * @see #RETRY
 * @see #WITH
 * @see #USE
 * @see #e4c_is_uncaught
 */
#define REACQUIRE(max_attempts, type, format, ...)                          \
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

/** @} */

/*
 * These undocumented macros hide implementation details.
 */

# define EXCEPTIONS4C_START_BLOCK(should_acquire)                           \
  for (                                                                     \
    EXCEPTIONS4C_SET_JUMP(e4c_start(should_acquire, EXCEPTIONS4C_DEBUG));   \
    e4c_next(EXCEPTIONS4C_DEBUG) || (                                       \
      e4c_is_uncaught() && (EXCEPTIONS4C_LONG_JUMP(e4c_get_env()), true)    \
    );                                                                      \
  )

#ifndef HAVE_SIGSETJMP
#define EXCEPTIONS4C_SET_JUMP(env) setjmp(*(env))
#define EXCEPTIONS4C_LONG_JUMP(env) longjmp(*(env), 1)
typedef jmp_buf e4c_env;
#else
#define EXCEPTIONS4C_SET_JUMP(env) sigsetjmp(*(env), 1)
#define EXCEPTIONS4C_LONG_JUMP(env) siglongjmp(*(env), 1)
typedef sigjmp_buf e4c_jump_buffer;
#endif

#ifndef NDEBUG
#define EXCEPTIONS4C_DEBUG __FILE__, __LINE__, __func__
#else
#define EXCEPTIONS4C_DEBUG NULL, 0, NULL
#endif

/**
 * Represents an exception type in the exception handling system
 *
 * When defining types of exceptions, they are given a *supertype* (that
 * organizes them hierarchically*) and a *default message*.
 *
 * ```c
 * const struct e4c_exception_type SimpleException = {NULL, "Simple exception"};
 * ```
 *
 * @see #e4c_exception
 * @see #THROW
 * @see #CATCH
 */
struct e4c_exception_type {

    /** The supertype of this exception type */
    const struct e4c_exception_type * supertype;

    /** The default message of this exception type */
    const char * default_message;
};

/**
 * Represents an instance of an exception type
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
 *   - The exception `name`
 *   - An *ad-hoc* `message` (as opposed to the *default* one)
 *   - The exact point of the program where it was thrown (source code `file`,
 *     `line` and `function` name, if available)
 *   - The value of the standard error code `errno` at the time the exception
 *     was thrown
 *   - The `cause` of the exception, which is the previous exception (if any),
 *     when the exception was thrown
 *   - The specific `type` of the exception, convenient when handling an
 *     abstract type of exceptions from a #CATCH block
 *   - Optional, *user-defined*, `custom_data`, which can be initialized and
 *     finalized throught context *handlers*
 *
 * @see #e4c_exception_type
 * @see #THROW
 * @see #CATCH
 * @see #e4c_get_exception
 */
struct e4c_exception {

    /** The type of this exception */
    const struct e4c_exception_type * type;

    /** The name of this exception */
    const char * name;

    /** The message of this exception */
    char message[256];

    /** The path of the source code file from which the exception was thrown */
    const char * file;

    /** The number of line from which the exception was thrown */
    int line;

    /** The function from which the exception was thrown */
    const char * function;

    /** The value of errno at the time the exception was thrown */
    int error_number;

    /** The cause of this exception */
    struct e4c_exception * cause;

    /** Custom data associated to this exception */
    void * custom_data;

    /* Number of times this exception is referenced */
    int _ref_count;
};

/** Represents the exception context of the running program */
struct e4c_context {

    /**
     * The current exception block of the running program
     *
     * It represents the innermost exception block.
     */
    void * current_block;

    /** The function to be executed whenever a new exception is thrown */
    void (*initialize_exception)(struct e4c_exception * exception);

    /** The function to be executed whenever an exception is destroyed */
    void (*finalize_exception)(const struct e4c_exception * exception);

    /** The function to be executed in the event of an uncaught exception */
    void (*uncaught_handler)(const struct e4c_exception * exception);
};

/**
 * @name Exception context handling functions
 *
 * These functions enclose the scope of the exception handling system and
 * retrieve the current exception context.
 *
 * @{
 */

/**
 * Sets the exception context supplier.
 *
 * @param supplier A function that supplies the current context supplier
 */
void e4c_set_context_supplier(struct e4c_context * (*supplier)(void));

/**
 * Retrieves the current context supplier.
 *
 * @return The current context supplier
 *
 * @see #e4c_set_context_supplier
 */
struct e4c_context * e4c_get_context(void);

/**
 * Returns whether the current exception block has an uncaught exception
 *
 * @return  `true` if the current exception block has an uncaught
 *          exception; otherwise `false`
 *
 * This function MAY be called during the execution of #FINALLY blocks.
 *
 * @see #FINALLY
 * @see #e4c_get_exception
 */
bool e4c_is_uncaught(void);

/**
 * Returns the exception that was thrown
 *
 * @return  The exception that was thrown in the current exception context (if
 *          any) otherwise `NULL`
 *
 * This function returns a pointer to the exception that was thrown in the
 * surrounding *exception-aware* block, if any; otherwise `NULL`.
 *
 * The thrown exception can be obtained any time, provided that the exception
 * context has begun at the time of the function call. However, it is sensible
 * to call this function only during the execution of #FINALLY or #CATCH
 * blocks.
 *
 * Moreover, a pointer to the thrown exception obtained *inside* a #FINALLY
 * or #CATCH block **must not** be used *outside* these blocks.
 *
 * The exception system objects are dinamically allocated and deallocated, as
 * the program enters or exits #TRY... #CATCH... #FINALLY blocks. While
 * it would be legal to *copy* the thrown exception and access to its `name`
 * and `message` outside these blocks, care should be taken in order not to
 * dereference the `cause` of the exception, unless it is a **deep copy**
 * (as opposed to a **shallow copy**).
 *
 * @see #e4c_exception
 * @see #THROW
 * @see #CATCH
 * @see #FINALLY
 */
const struct e4c_exception * e4c_get_exception(void);

/** @} */

/*
 * These functions SHOULD be called only via provided macros.
 */

e4c_env * e4c_start(bool should_acquire, const char * file, int line, const char * function);
bool e4c_acquire(const char * file, int line, const char * function);
bool e4c_try(const char * file, int line, const char * function);
bool e4c_dispose(const char * file, int line, const char * function);
bool e4c_catch(const struct e4c_exception_type * type, const char * file, int line, const char * function);
bool e4c_finally(const char * file, int line, const char * function);
bool e4c_next(const char * file, int line, const char * function);
e4c_env * e4c_get_env(void);
e4c_env * e4c_restart(bool should_reacquire, int max_attempts, const struct e4c_exception_type * type, const char * name, const char * file, int line, const char * function, const char * format, ...);
e4c_env * e4c_throw(const struct e4c_exception_type * type, const char * name, const char * file, int line, const char * function, const char * format, ...);

# endif
