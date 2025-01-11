/**
 *
 * @file        e4c.h
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
 *   - #E4C_TRY
 *   - #E4C_CATCH
 *   - #E4C_FINALLY
 *   - #E4C_THROW
 *   - #E4C_WITH
 *   - #E4C_USING
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
#include <stdnoreturn.h>

#ifndef __bool_true_false_are_defined
#include <stdbool.h>
#endif

#ifdef HAVE_SIGSETJMP
typedef sigjmp_buf e4c_jump_buffer;
#define EXCEPTIONS4C_SET_JUMP(buffer) sigsetjmp(buffer, 1)
#define EXCEPTIONS4C_LONG_JUMP(buffer) siglongjmp(buffer, 1)
#else
typedef jmp_buf e4c_jump_buffer;
#define EXCEPTIONS4C_SET_JUMP(buffer) setjmp(buffer)
#define EXCEPTIONS4C_LONG_JUMP(buffer) longjmp(buffer, 1)
#endif


# ifndef NDEBUG
#   define E4C_DEBUG_INFO               __FILE__, __LINE__, __func__
# else
#   define E4C_DEBUG_INFO               NULL, 0, NULL
# endif

/*
 * These undocumented macros hide implementation details from documentation.
 */

# define EXCEPTIONS4C_BLOCK(stage)                                          \
    if (EXCEPTIONS4C_SET_JUMP(*e4c_start(stage, E4C_DEBUG_INFO)) >= 0)      \
        while (e4c_next_stage())

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
 * A #E4C_TRY statement executes a block of code. If an exception is thrown and
 * there is a #E4C_CATCH block that can handle it, then control will be
 * transferred to it. If there is a #E4C_FINALLY block, then it will be executed,
 * no matter whether the `try` block completes normally or abruptly, and no
 * matter whether a `catch` block is first given control.
 *
 * The block of code immediately after the keyword `try` is called **the `try`
 * block** of the `try` statement. The block of code immediately after the
 * keyword `finally` is called **the `finally` block** of the `try` statement.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   stack_t * stack = stack_new();
 *   try{
 *       // the try block
 *       int value = stack_pop(stack);
 *       stack_push(stack, 16);
 *       stack_push(stack, 32);
 *   }catch(StackOverflowException){
 *       // a catch block
 *       printf("Could not push.");
 *   }catch(StackUnderflowException){
 *       // another catch block
 *       printf("Could not pop.");
 *   }finally{
 *       // the finally block
 *       stack_delete(stack);
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * One `try` block may precede many `catch` blocks (also called *exception
 * handlers*). A `catch` block **must** have exactly one parameter, which is the
 * exception type it is capable of handling. Within the `catch` block, the
 * exception can be accessed through the function #e4c_get_exception.
 * Exception handlers are considered in left-to-right order: the earliest
 * possible `catch` block handles the exception. If no `catch` block can handle
 * the thrown exception, it will be *propagated*.
 *
 * Sometimes it may come in handy to #E4C_RETRY an entire `try` block; for
 * instance, once the exception has been caught and the error condition has been
 * solved.
 *
 * A `try` block has an associated [status](#e4c_status) according to the
 * way it has been executed:
 *
 *   - It *succeeds* when the execution reaches the end of the block without
 *     any exceptions.
 *   - It *recovers* when an exception is thrown but a `catch` block handles it.
 *   - It *fails* when an exception is thrown and it's not caught.
 *
 * The status of the current `try` block can be retrieved through the function
 * #e4c_get_status.
 *
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to using
 *     the keyword `try`. Such programming error will lead to an abrupt exit of
 *     the program (or thread).
 *   - A `try` block **must** precede, at least, another block of code,
 *     introduced by either `catch` or `finally`.
 *     - A `try` block may precede several `catch` blocks.
 *     - A `try` block can precede, at most, one `finally` block.
 *   - A `try` block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return` (but it is legal to #E4C_THROW an exception).
 *
 * @post
 *   - A `finally` block will be executed after the `try` block and any `catch`
 *     block that might be executed, no matter whether the `try` block
 *     *succeeds*, *recovers* or *fails*.
 *
 * @see     #E4C_CATCH
 * @see     #E4C_FINALLY
 * @see     #E4C_RETRY
 * @see     #e4c_status
 * @see     #e4c_get_status
 */
#define E4C_TRY                                                             \
  EXCEPTIONS4C_BLOCK(e4c_acquiring)                                         \
  if (e4c_get_current_stage() == e4c_trying && e4c_next_stage())
    /* simple optimization: e4c_next_stage will avoid disposing stage */

/**
 * Introduces a block of code capable of handling a specific type of exceptions
 *
 * @param   exception_type
 *          The type of exceptions to be handled
 *
 * #E4C_CATCH blocks are optional code blocks that **must** be preceded by #E4C_TRY,
 * #E4C_WITH... #E4C_USE or #E4C_USING blocks. Several `catch` blocks can be placed
 * next to one another.
 *
 * When an exception is thrown, the system looks for a `catch` block to handle
 * it. The first capable block (in order of appearance) will be executed. The
 * exception is said to be *caught* and the `try` block is in *recovered*
 * (status)[@ ref e4c_status].
 *
 * The caught exception can be accessed through the function #e4c_get_exception.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   try{
 *       ...
 *   }catch(RuntimeException){
 *       const e4c_exception * exception = e4c_get_exception();
 *       printf("Error: %s", exception->message);
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The actual type of the exception can be checked against other exception types
 * through the function #e4c_is_instance_of.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   try{
 *       ...
 *   }catch(RuntimeException){
 *       const e4c_exception * exception = e4c_get_exception();
 *       if( e4c_is_instance_of(exception, &SignalException) ){
 *           // the exception type is SignalException or any subtype
 *       }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The type might also be compared directly against another specific exception
 * type.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   try{
 *       ...
 *   }catch(RuntimeException){
 *       const e4c_exception * exception = e4c_get_exception();
 *       if(exception->type == &NullPointerException){
 *           // the exception type is precisely NullPointerException
 *       }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * After the `catch` block completes, the #E4C_FINALLY block (if any) is executed.
 * Then the program continues by the next line following the set of `try`...
 * `catch`... `finally` blocks.
 *
 * However, if an exception is thrown in a `catch` block, then the `finally`
 * block will be executed right away and the system will look for an outter
 * `catch` block to handle it.
 *
 * Only one of all the `catch` blocks will be executed for each `try` block,
 * even though the executed `catch` block throws another exception. The only
 * possible way to execute more than one `catch` block would be by [retrying](#E4C_RETRY)
 * the entire `try` block.
 *
 * @pre
 *   - A `catch` block **must** be preceded by one of these blocks:
 *     - A #E4C_TRY block
 *     - A #E4C_WITH... #E4C_USE block
 *     - A #E4C_USING block
 *     - Another #E4C_CATCH block
 *   - A `catch` block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return` (but it is legal to #E4C_THROW an exception).
 *
 * @see     #E4C_TRY
 * @see     #e4c_exception_type
 * @see     #e4c_get_exception
 * @see     #e4c_exception
 * @see     #e4c_is_instance_of
 */
#define E4C_CATCH(exception_type)                                           \
  else if (e4c_catch(&exception_type))

/**
 * Introduces a block of code responsible for cleaning up the previous
 * exception-aware block
 *
 * #E4C_FINALLY blocks are optional code blocks that **must** be preceded by
 * #E4C_TRY, #E4C_WITH... #E4C_USE or #E4C_USING blocks. It is allowed to place, at
 * most, one `finally` block for each one of these.
 *
 * The `finally` block can determine the completeness of the *exception-aware*
 * block through the function #e4c_get_status. The thrown exception (if any)
 * can also be accessed through the function #e4c_get_exception.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   try{
 *      ...
 *   }finally{
 *      switch( e4c_get_status() ){
 *
 *          case e4c_succeeded:
 *              ...
 *              break;
 *
 *          case e4c_recovered:
 *              ...
 *              break;
 *
 *          case e4c_failed:
 *              ...
 *              break;
 *      }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The finally block will be executed only **once**. The only possible way to
 * execute it again would be by [retrying](#E4C_RETRY) the entire #E4C_TRY block.
 *
 * @pre
 *   - A `finally` block **must** be preceded by a `try`, `with`... `use`,
 *     `using` or `catch` block.
 *   - A `finally` block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return` (but it is legal to #E4C_THROW an exception).
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to using
 *     the keyword `finally`. Such programming error will lead to an abrupt exit
 *     of the program (or thread).
 *
 * @see     #e4c_exception
 * @see     #e4c_get_exception
 * @see     #e4c_get_status
 * @see     #e4c_status
 */
#define E4C_FINALLY                                                         \
  else if (e4c_get_current_stage() == e4c_finalizing)

/**
 * Repeats the previous #E4C_TRY (or #E4C_USE) block entirely
 *
 * @param   max_retry_attempts
 *          The maximum number of attempts to retry
 * @param   exception_type
 *          The type of exception to be thrown
 * @param   format
 *          The detail message.
 * @param   ...
 *          The variadic arguments that will be formatted according to the
 *          format control
 *
 * This macro repeats the previous `try` or `use` block, up to a specified
 * maximum number of attempts. If the block has already been *tried* at
 * least the specified number of times, then the supplied exception will
 * be thrown.
 *
 * It is intended to be used within #E4C_CATCH or #E4C_FINALLY blocks as
 * a quick way to fix an error condition and *try again*.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   const char * file_path = config_get_user_defined_file_path();
 *
 *   try{
 *       config = read_config(file_path);
 *   }catch(ConfigException){
 *       file_path = config_get_default_file_path();
 *       retry(1, ConfigException, "Wrong defaults.");
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note
 * #E4C_TRY MAY be used at a `finally` block.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   int dividend = 100;
 *   int divisor = 0;
 *   int result = 0;
 *
 *   try{
 *       result = dividend / divisor;
 *       do_something(result);
 *   }finally{
 *       if( e4c_get_status() == e4c_failed ){
 *           divisor = 1;
 *           retry(1, RuntimeException, "Retry Error");
 *       }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to using
 *     the keyword `retry`. Such programming error will lead to an abrupt exit
 *     of the program (or thread).
 *   - The `retry` keyword **must** be used from a `catch` or `finally` block.
 * @post
 *   - Control does not return to the `retry` point.
 *
 * @see     #E4C_REACQUIRE
 * @see     #E4C_TRY
 * @see     #E4C_USE
 */
#define E4C_RETRY(max_retry_attempts, exception_type, format, ...)          \
  e4c_restart(                                                              \
    false,                                                                  \
    max_retry_attempts,                                                     \
    &exception_type,                                                        \
    E4C_DEBUG_INFO,                                                         \
    (format)                                                                \
    __VA_OPT__(,) __VA_ARGS__                                               \
  )

/**
 * Signals an exceptional situation represented by an exception object
 *
 * @param   exception_type
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
 * appropriate #E4C_CATCH block that can handle the exception. The system unwinds
 * the call chain of the program and executes the #E4C_FINALLY blocks it finds.
 *
 * When no `catch` block is able to handle an exception, the system eventually
 * gets to the main function of the program. This situation is called an
 * **uncaught exception**.
 *
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to using
 *     the keyword `throw`. Such programming error will lead to an abrupt exit
 *     of the program (or thread).
 * @post
 *   - Control does not return to the `throw` point.
 *
 * @see     #e4c_exception_type
 * @see     #e4c_exception
 * @see     #e4c_uncaught_handler
 * @see     #e4c_get_exception
 */
#define E4C_THROW(exception_type, format, ...)                              \
  e4c_throw(                                                                \
    &exception_type,                                                        \
    E4C_DEBUG_INFO,                                                         \
    (format)                                                                \
    __VA_OPT__(,) __VA_ARGS__                                               \
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
 * The combination of keywords `with`... #E4C_USE encapsules the *Dispose
 * Pattern*. This pattern consists of two separate blocks and an implicit call
 * to a given function:
 *
 *   - the `with` block is responsible for the resource acquisition
 *   - the `use` block makes use of the resource
 *   - the disposal function will be called implicitly
 *
 * A `with` block **must** be followed by a `use` block. In addition, the `use`
 * block may be followed by several #E4C_CATCH blocks and/or one #E4C_FINALLY block.
 *
 * The `with` keyword guarantees that the disposal function will be called **if,
 * and only if**, the acquisition block is *completed* without any errors (i.e.
 * the acquisition block does not #E4C_THROW any exceptions).
 *
 * If the `with` block does not complete, neither the disposal function nor the
 * `use` block will be executed.
 *
 * The disposal function is called right after the `use` block. If an exception
 * is thrown, the `catch` or `finally` blocks (if any) will take place **after**
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   # define e4c_dispose_file(file , failed) fclose(file)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Here is the typical usage of `with`... `use`:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   with(foo, e4c_dispose_foo){
 *       foo = e4c_acquire_foo(foobar);
 *       validate_foo(foo, bar);
 *       ...
 *   }use{
 *       do_something(foo);
 *       ...
 *   }catch(FooAcquisitionFailed){
 *       recover1(foobar);
 *       ...
 *   }catch(SomethingElseFailed){
 *       recover2(foo);
 *       ...
 *   }finally{
 *       cleanup();
 *       ...
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Nonetheless, *one-liners* fit nicely too:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   with(bar, e4c_dispose_bar) bar = e4c_acquire_bar(baz, 123); use foo(bar);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * There is a way to lighten up even more this pattern by defining convenience
 * macros, customized for a specific kind of resources. For example,
 * `e4c_using_file` or `e4c_using_memory`.
 *
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to using
 *     the keyword `with`. Such programming error will lead to an abrupt exit of
 *     the program (or thread).
 *   - A `with` block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return` (but it is legal to `throw` an exception).
 *   - A `with` block **must** always be followed by a `use` block.
 *
 * @see     #E4C_USE
 * @see     #E4C_USING
 */
#define E4C_WITH(resource, dispose)                                         \
  EXCEPTIONS4C_BLOCK(e4c_beginning)                                         \
  if (e4c_get_current_stage() == e4c_disposing) {                           \
    dispose((resource), e4c_get_status() == e4c_failed);                    \
  } else if (e4c_get_current_stage() == e4c_acquiring) {

/**
 * Closes a block of code with automatic disposal of a resource
 *
 * A #E4C_USE block **must** always be preceded by a #E4C_WITH block. These two
 * keywords are designed so the compiler will complain about *dangling*
 * #E4C_WITH... #E4C_USE blocks.
 *
 * A code block introduced by the `use` keyword will only be executed *if, and
 * only if*, the acquisition of the resource *completes* without exceptions.
 *
 * The disposal function will be executed, either if the `use` block completes
 * or not.
 *
 * @pre
 *   - A #E4C_USE block **must** be preceded by a #E4C_WITH block.
 *   - A #E4C_USE block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return`  (but it is legal to #E4C_THROW an exception).
 *
 * @see     #E4C_WITH
 */
#define E4C_USE                                                             \
  } else if (e4c_get_current_stage() == e4c_trying)

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
 * blocks introduced by #E4C_WITH... #E4C_USE. For example, a #E4C_USING block can also
 * precede #E4C_CATCH and #E4C_FINALLY blocks.
 *
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to using
 *     the keyword `using`. Such programming error will lead to an abrupt exit
 *     of the program (or thread).
 *   - A `using` block **must not** be exited through any of: `goto`, `break`,
 *     `continue` or `return`  (but it is legal to #E4C_THROW an exception).
 *
 * @see     #E4C_WITH
 */
#define E4C_USING(type, resource, args)                                     \
  E4C_WITH((resource), e4c_dispose_##type) {                                \
    (resource) = e4c_acquire_##type args;                                   \
  } E4C_USE

/**
 * Repeats the previous #E4C_WITH block entirely
 *
 * @param   max_reacquire_attempts
 *          The maximum number of attempts to reacquire
 * @param   exception_type
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
 * It is intended to be used within #E4C_CATCH or #E4C_FINALLY blocks, next to a
 * #E4C_WITH... #E4C_USE or #E4C_USING block, when the resource acquisition fails,
 * as a quick way to fix an error condition and try to acquire the resource
 * again.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   image_type * image;
 *   const char * image_path = image_get_user_avatar();
 *
 *   with(image, e4c_image_dispose){
 *       image = e4c_image_acquire(image_path);
 *   }use{
 *       image_show(image);
 *   }catch(ImageNotFoundException){
 *       image_path = image_get_default_avatar();
 *       reacquire(1);
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Once the resource has been acquired, the `use` block can also be repeated
 * *alone* through the keyword #E4C_RETRY:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   image_type * image;
 *   const char * image_path = image_get_user_avatar();
 *   display_type * display = display_get_user_screen();
 *
 *   with(image, e4c_image_dispose){
 *       image = e4c_image_acquire(image_path);
 *   }use{
 *       image_show(image, display);
 *   }catch(ImageNotFoundException){
 *       image_path = image_get_default_avatar();
 *       reacquire(1);
 *   }catch(DisplayException){
 *       display = display_get_default_screen();
 *       retry(1);
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @pre
 *   - The `reacquire` keyword **must** be used from a `catch` or `finally`
 *     block, preceded by `with`... `use` or `using` blocks.
 *   - A program (or thread) **must** begin an exception context prior to using
 *     the keyword `reacquire`. Such programming error will lead to an abrupt
 *     exit of the program (or thread).
 * @post
 *   - Control does not return to the `reacquire` point.
 *
 * @see     #E4C_RETRY
 * @see     #E4C_WITH
 * @see     #E4C_USE
 */
#define E4C_REACQUIRE(max_reacquire_attempts, exception_type, format, ...)  \
  e4c_restart(                                                              \
    true,                                                                   \
    max_reacquire_attempts,                                                 \
    &exception_type,                                                        \
    E4C_DEBUG_INFO,                                                         \
    (format)                                                                \
    __VA_OPT__(,) __VA_ARGS__                                               \
  )

/** @} */

/**
 * @name Integration macros
 *
 * These macros are designed to ease the integration of external libraries which
 * make use of the exception handling system.
 *
 * @{
 */

/** @} */

/**
 * @name Other convenience macros
 *
 * @{
 */

/**
 * Defines an exception type
 *
 * @param   name
 *          Name of the new exception type
 * @param   default_message
 *          Default message of the new exception type
 * @param   supertype
 *          Supertype of the new exception type
 *
 * This macro allocates a new, `const` [exception type]
 * (#e4c_exception_type).
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   E4C_DEFINE_EXCEPTION(StackException, "Stack exception", RuntimeException);
 *   E4C_DEFINE_EXCEPTION(StackOverflowException, "Stack overflow", StackException);
 *   E4C_DEFINE_EXCEPTION(StackUnderflowException, "Stack underflow", StackException);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This macro is intended to be used inside sorce code files.
 *
 * @see     #e4c_exception_type
 * @see     #RuntimeException
 */
# define E4C_DEFINE_EXCEPTION(name, default_message, supertype) \
    \
    const struct e4c_exception_type name = { \
        #name, \
        default_message, \
        &supertype \
    }

/** @} */


/** Represents a message exception */
typedef char e4c_exception_message[128];

/**
 * Represents an exception type in the exception handling system
 *
 * The types of the exceptions a program will use are **defined** in source code
 * files through the macro #E4C_DEFINE_EXCEPTION.
 *
 * When defining types of exceptions, they are given a *name*, a *default
 * message* and a *supertype* to organize them into a *pseudo-hierarchy*:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   E4C_DEFINE_EXCEPTION(SimpleException, "Simple exception", RuntimeException);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Exceptions are defined as global objects. There is a set of predefined
 * exceptions built into the framework; #RuntimeException is the *root* of the
 * exceptions *pseudo-hierarchy*:
 *
 *   - #RuntimeException
 *     - #NullPointerException
 *
 * @see     #e4c_exception
 * @see     #E4C_DEFINE_EXCEPTION
 * @see     #E4C_THROW
 * @see     #E4C_CATCH
 */
struct e4c_exception_type {

    /** The name of this exception type */
    const char * name;

    /** The default message of this exception type */
    const e4c_exception_message default_message;

    /** The supertype of this exception type */
    const struct e4c_exception_type * supertype;
};

/**
 * Represents an instance of an exception type
 *
 * Exceptions are a means of breaking out of the normal flow of control of a
 * code block in order to handle errors or other exceptional conditions. An
 * exception should be [thrown](#E4C_THROW) at the point where the error is
 * detected; it may be [handled](#E4C_CATCH) by the surrounding code block or by
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
 *     abstract type of exceptions from a #E4C_CATCH block
 *   - Optional, *user-defined*, `custom_data`, which can be initialized and
 *     finalized throught context *handlers*
 *
 * @note
 * **Any** exception can be caught by a block introduced by
 * `catch(RuntimeException)`.
 *
 * @see     #e4c_exception_type
 * @see     #E4C_THROW
 * @see     #E4C_CATCH
 * @see     #e4c_get_exception
 * @see     #e4c_context_set_handlers
 * @see     #RuntimeException
 */
struct e4c_exception {

    /* This field is undocumented on purpose and reserved for internal use */
    int                             _;

    /** The name of this exception */
    const char *                    name;

    /** The message of this exception */
    e4c_exception_message           message;

    /** The path of the source code file from which the exception was thrown */
    const char *                    file;

    /** The number of line from which the exception was thrown */
    int                             line;

    /** The function from which the exception was thrown */
    const char *                    function;

    /** The value of errno at the time the exception was thrown */
    int                             error_number;

    /** The type of this exception */
    const struct e4c_exception_type * type;

    /** The cause of this exception */
    struct e4c_exception * cause;

    /** Custom data associated to this exception */
    void *                          custom_data;
};

/**
 * Represents the completeness of a code block aware of exceptions
 *
 * The symbolic values representing the status of a block help to distinguish
 * between different possible situations inside a #E4C_FINALLY block. For example,
 * different cleanup actions can be taken, depending on the status of the block.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   try{
 *      ...
 *   }finally{
 *      switch( e4c_get_status() ){
 *
 *          case e4c_succeeded:
 *              ...
 *              break;
 *
 *          case e4c_recovered:
 *              ...
 *              break;
 *
 *          case e4c_failed:
 *              ...
 *              break;
 *      }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @see     #e4c_get_status
 * @see     #E4C_FINALLY
 */
enum e4c_status {

    /** There were no exceptions */
    e4c_succeeded,

    /** There was an exception, but it was caught */
    e4c_recovered,

    /** There was an exception and it wasn't caught */
    e4c_failed
};

/**
 * Represents a function which will be executed in the event of an uncaught
 * exception.
 *
 * @param   exception
 *          The uncaught exception
 *
 * This handler can be set through the function #e4c_context_set_handlers:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   void my_uncaught_handler(const e4c_exception * exception){
 *
 *       printf("Error: %s (%s)\n", exception->name, exception->message);
 *   }
 *
 *   int main(int argc, char * argv[]){
 *
 *       E4C_USING_CONTEXT(true){
 *
 *           e4c_context_set_handlers(my_uncaught_handler, NULL, NULL, NULL);
 *           // ...
 *       }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * There exists a convenience function #e4c_print_exception which is used as
 * the default *uncaught handler*, unless otherwise specified. It simply prints
 * information regarding the exception to `stderr`.
 *
 * @warning
 * An uncaught handler is not allowed to try and recover the current exception
 * context. Moreover, the program (or current thread) will terminate right after
 * the function returns.
 *
 * @see     #e4c_context_set_handlers
 * @see     #e4c_initialize_handler
 * @see     #e4c_finalize_handler
 * @see     #e4c_print_exception
 */
typedef void (*e4c_uncaught_handler)(const struct e4c_exception * exception);

/**
 * Represents a function which will be executed whenever a new exception is
 * thrown.
 *
 * @param   exception
 *          The newly thrown exception
 *
 * When this handler is set, it will be called any time a new exception is
 * created. The `void` pointer returned by this function will be assigned to
 * the exception's *custom_data*. This data can be accessed later on, for
 * example, from a #E4C_CATCH block, or an *uncaught handler*, for any
 * specific purpose.
 *
 * This handler can be set through the function #e4c_context_set_handlers:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   void * my_initialize_handler(const e4c_exception * e){
 *
 *       if( e4c_is_instance_of(e, &SignalException) ){
 *           printf("Program received signal %s (%d)!\n", e->file, e->line);
 *       }
 *
 *       return(NULL);
 *   }
 *
 *   int main(int argc, char * argv[]){
 *
 *       E4C_USING_CONTEXT(true){
 *
 *           e4c_context_set_handlers(NULL, NULL, my_initialize_handler, NULL);
 *           // ...
 *       }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * By the time this handler is called, the exception already has been assigned
 * the initial value specified for `custom_data`, so the handler may make use
 * of it:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   void * log_handler(const e4c_exception * e){
 *
 *       printf("LOG: Exception thrown at module '%s'\n", e->custom_data);
 *
 *       return(NULL);
 *   }
 *
 *   int main(int argc, char * argv[]){
 *
 *       E4C_USING_CONTEXT(true){
 *
 *           e4c_context_set_handlers(NULL, "FOO", log_handler, NULL);
 *           // ...
 *       }
 *
 *       E4C_USING_CONTEXT(true){
 *
 *           e4c_context_set_handlers(NULL, "BAR", log_handler, NULL);
 *           // ...
 *       }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @see     #e4c_context_set_handlers
 * @see     #e4c_exception
 * @see     #e4c_finalize_handler
 * @see     #e4c_uncaught_handler
 * @see     #e4c_print_exception
 */
typedef void * (*e4c_initialize_handler)(const struct e4c_exception * exception);

/**
 * Represents a function which will be executed whenever an exception is
 * destroyed.
 *
 * @param   custom_data
 *          The "custom data" of the exception to be discarded
 *
 * When this handler is set, it will be called any time an exception is
 * discarded. It will be passed the *custom_data* of the exception, so it may
 * dispose resources previously acquired by the *initialize handler*.
 *
 * This handler can be set through the function #e4c_context_set_handlers:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   void * initialize_data(const e4c_exception * exception){
 *
 *       const char * custom_data = malloc(1024);
 *
 *       if(custom_data != NULL){
 *           if( e4c_is_instance_of(exception, &SignalException) ){
 *               strcpy(custom_data, "SIGNAL ERROR");
 *           }else{
 *               strcpy(custom_data, "RUNTIME ERROR");
 *           }
 *       }
 *
 *       return(custom_data);
 *   }
 *
 *   void finalize_data(void * custom_data){
 *
 *       free(custom_data);
 *   }
 *
 *   int main(int argc, char * argv[]){
 *
 *       E4C_USING_CONTEXT(true){
 *
 *           e4c_context_set_handlers(NULL, NULL, initialize_data, finalize_data);
 *           ...
 *       }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @see     #e4c_context_set_handlers
 * @see     #e4c_exception
 * @see     #e4c_finalize_handler
 * @see     #e4c_uncaught_handler
 * @see     #e4c_print_exception
 */
typedef void (*e4c_finalize_handler)(void * custom_data);

/** Represents the execution stage of the current exception frame */
enum e4c_frame_stage {
    e4c_beginning,
    e4c_acquiring,
    e4c_trying,
    e4c_disposing,
    e4c_catching,
    e4c_finalizing,
    e4c_done
};

/** Represents an exception frame */
struct e4c_frame {
    struct e4c_frame *          previous;
    enum e4c_frame_stage        stage;
    bool                        uncaught;
    struct e4c_exception *      thrown_exception;
    int                         retry_attempts;
    int                         reacquire_attempts;
    e4c_jump_buffer             continuation;
};

/** Represents an exception context */
struct e4c_context {
    struct e4c_frame *          current_frame;
    e4c_uncaught_handler        uncaught_handler;
    void *                      custom_data;
    e4c_initialize_handler      initialize_handler;
    e4c_finalize_handler        finalize_handler;
};

/**
 * Represents the function that returns the current exception context.
 */
typedef struct e4c_context * (*e4c_context_supplier)(void);

/**
 * @name Predefined exceptions
 *
 * Built-in exceptions represent usual error conditions that might occur during
 * the course of any program.
 *
 * They are organized into a *pseudo-hierarchy*; #RuntimeException is the
 * common *supertype* of all exceptions.
 *
 * @{
 */

/**
 * This is the root of the exception *pseudo-hierarchy*
 *
 * #RuntimeException is the common *supertype* of all exceptions.
 *
 * @par     Direct known subexceptions:
 *          #NullPointerException
 */
extern const struct e4c_exception_type RuntimeException;

/**
 * This exception is thrown when an unexpected null pointer is found
 *
 * #NullPointerException is thrown when some part of the program gets a
 * pointer which was expected or required to contain a valid memory address.
 *
 * @par     Extends:
 *          #RuntimeException
 */
extern const struct e4c_exception_type NullPointerException;

/** @} */

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
 * @param context_supplier The context supplier
 */
void e4c_set_context_supplier(e4c_context_supplier supplier);

/**
 * Sets the optional handlers of an exception context
 *
 * @param   uncaught_handler
 *          The function to be executed in the event of an uncaught exception
 * @param   custom_data
 *          The initial value assigned to the custom_data of a new exception
 * @param   initialize_handler
 *          The function to be executed whenever a new exception is thrown
 * @param   finalize_handler
 *          The function to be executed whenever an exception is destroyed
 *
 * These handlers are a means of customizing the behavior of the exception
 * system. For example, you can specify what needs to be done when a thrown
 * exception is not caught (and thus, the program or thread is about to end) by
 * calling `e4c_context_set_handlers` with your own [uncaught handler]
 * (#e4c_uncaught_handler).
 *
 * You can also control the [custom data](#e4c_exception::custom_data)
 * attached to any new exception by specifying any or all of these:
 *
 *   - The *initial value* to be assigned to the `custom_data`
 *   - The function to *initialize* the `custom_data`
 *   - The function to *finalize* the `custom_data`
 *
 * When these handlers are defined, they will be called anytime an exception is
 * uncaught, created or destroyed. You can use them to meet your specific needs.
 * For example, you could...
 *
 *   - ...send an e-mail whenever an exception is uncaught
 *   - ...log any thrown exception to a file
 *   - ...capture the call stack in order to print it later on
 *   - ...go for something completely different ;)
 *
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to
 *     calling `e4c_context_set_handlers`. Such programming error will lead to
 *     an abrupt exit of the program (or thread).
 *
 * @see     #e4c_uncaught_handler
 * @see     #e4c_initialize_handler
 * @see     #e4c_finalize_handler
 * @see     #e4c_exception
 * @see     #e4c_print_exception
 */
void e4c_context_set_handlers(
    e4c_uncaught_handler uncaught_handler,
    void * custom_data,
    e4c_initialize_handler initialize_handler,
    e4c_finalize_handler finalize_handler
);

/**
 * Returns the completeness status of the executing code block
 *
 * @return  The completeness status of the executing code block
 *
 * Exception-aware code blocks have a completeness status regarding the
 * exception handling system. This status determines whether an exception was
 * actually thrown or not, and whether the exception was caught or not.
 *
 * The status of the current block can be obtained any time, provided that the
 * exception context has begun at the time of the function call. However, it is
 * sensible to call this function only during the execution of #E4C_FINALLY
 * blocks.
 *
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to
 *     calling `e4c_get_status`. Such programming error will lead to an abrupt
 *     exit of the program (or thread).
 *
 * @see     #e4c_status
 * @see     #E4C_FINALLY
 */
enum e4c_status e4c_get_status(void);

/**
 * Returns the exception that was thrown
 *
 * @return  The exception that was thrown in the current exception context (if
 *          any) otherwise `NULL`
 *
 * This function returns a pointer to the exception that was thrown in the
 * surrounding *exception-aware* block, if any; otherwise `NULL`.
 *
 * The function #e4c_is_instance_of will determine if the thrown exception is
 * an instance of any of the defined exception types. The `type` of the thrown
 * exception can also be compared for an exact type match.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   try{
 *      ...
 *   }catch(RuntimeException){
 *      const e4c_exception * exception = e4c_get_exception();
 *      if( e4c_is_instance_of(exception, &SignalException) ){
 *          ...
 *      }else if(exception->type == &NullPointerException){
 *          ...
 *      }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The thrown exception can be obtained any time, provided that the exception
 * context has begun at the time of the function call. However, it is sensible
 * to call this function only during the execution of #E4C_FINALLY or #E4C_CATCH
 * blocks.
 *
 * Moreover, a pointer to the thrown exception obtained *inside* a #E4C_FINALLY
 * or #E4C_CATCH block **must not** be used *outside* these blocks.
 *
 * The exception system objects are dinamically allocated and deallocated, as
 * the program enters or exits #E4C_TRY... #E4C_CATCH... #E4C_FINALLY blocks. While
 * it would be legal to *copy* the thrown exception and access to its `name`
 * and `message` outside these blocks, care should be taken in order not to
 * dereference the `cause` of the exception, unless it is a **deep copy**
 * (as opposed to a **shallow copy**).
 *
 * @pre
 *   - A program (or thread) **must** begin an exception context prior to
 *     calling `e4c_get_exception`. Such programming error will lead to an
 *     abrupt exit of the program (or thread).
 *
 * @see     #e4c_exception
 * @see     #e4c_is_instance_of
 * @see     #E4C_THROW
 * @see     #E4C_CATCH
 * @see     #E4C_FINALLY
 */
const struct e4c_exception * e4c_get_exception(void);

/** @} */

/**
 * @name Other integration and convenience functions
 *
 * @{
 */

/**
 * Gets the library major version number
 *
 * @return  The major version number associated with the library
 *
 * This function provides the same information as #EXCEPTIONS4C_VERSION,
 * but the returned version number is associated with the actual,
 * compiled library.
 *
 * @note
 * This version number can be considered as the *run-time* library version
 * number, as opposed to the *compile-time* library version number (specified by
 * the header file).
 *
 * @remark
 * The library **must** be compiled with the corresponding header (i.e. library
 * version number should be equal to header version number).
 *
 * @see     #EXCEPTIONS4C_VERSION
 */
int e4c_library_version(void);

/**
 * Returns whether an exception instance is of a given type
 *
 * @param   instance
 *          The thrown exception
 * @param   exception_type
 *          A previously defined type of exceptions
 * @return  Whether the specified exception is an instance of the given type
 *
 * #e4c_is_instance_of can be used to determine if a thrown exception **is an
 * instance of** a given exception type.
 *
 * This function is intended to be used in a #E4C_CATCH block, or in a #E4C_FINALLY
 * block provided that some exception was actually thrown (i.e.
 * #e4c_get_status returs #e4c_failed or #e4c_recovered).
 *
 * This function will return `false` if either `instance` or `type` are
 * `NULL`.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *   try{
 *      ...
 *   }catch(RuntimeException){
 *      const e4c_exception * exception = e4c_get_exception();
 *      if( e4c_is_instance_of(exception, &SignalException) ){
 *          ...
 *      }else if(exception->type == &NullPointerException){
 *          ...
 *      }
 *   }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @pre
 *   - `instance` **must not** be `NULL`
 *   - `type` **must not** be `NULL`
 *
 * @see     #e4c_exception
 * @see     #e4c_exception_type
 * @see     #e4c_get_exception
 */
bool e4c_is_instance_of(
    const struct e4c_exception * instance,
    const struct e4c_exception_type * exception_type
);

/**
 * Prints the supplied exception to the standard error output
 *
 * @param   exception
 *          The exception to print
 *
 * This is a convenience function for showing an error message through the
 * standard error output. It will be used by default as the handler for
 * uncaught exceptions.
 *
 * @see     #e4c_uncaught_handler
 */
void e4c_print_exception(const struct e4c_exception * exception);

/** @} */

/*
 * Next functions are undocumented on purpose, because they shouldn't be used
 * directly (but through the "keywords").
 */

e4c_jump_buffer * e4c_start(
    enum e4c_frame_stage        stage,
    const char *                file,
    int                         line,
    const char *                function
);

bool e4c_next_stage(void);

enum e4c_frame_stage e4c_get_current_stage(void);

bool e4c_catch(const struct e4c_exception_type * exception_type);

noreturn void e4c_restart(
    bool                        should_reacquire,
    int                         max_repeat_attempts,
    const struct e4c_exception_type * exception_type,
    const char *                file,
    int                         line,
    const char *                function,
    const char *                format,
    ...
);

noreturn void e4c_throw(
    const struct e4c_exception_type * exception_type,
    const char *                file,
    int                         line,
    const char *                function,
    const char *                format,
    ...
);


# endif
