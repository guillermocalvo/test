
<div style="justify-items: center">
<img style="display: block" src="exceptions4c-logo.svg">
</div>

# Introduction

Bring the power of exceptions to your C applications!

![][EXAMPLE]

> [!NOTE]
> This library provides you with a set of macros and functions that map the exception handling semantics you are probably
already used to.


# Getting Started

## Adding exceptions4c to Your Project

This library consists of two files:

- exceptions4c.h
- exceptions4c.c

To use the library in your project, include the header file in your source code files.

```c
#include <exceptions4c.h>
```

And then link your program against the library code.

> [!NOTE]
> There is also a lightweight version of this library, intended for small projects and embedded systems.
> [exceptions4c-lite][EXCEPTIONS4C_LITE] is a header-only library that provides the
> core functionality of exceptions4c in just one header file.

## Defining Exception Types

Create meaningful exceptions that reflect problematic situations.

@snippet pet-store.c exception_types

A program must define the types of errors that can be raised and handled.

> [!NOTE]
> Exception types may have a supertype, creating a hierarchy, where a more specific type can be built upon a more
> generic one.


# Basic Usage

Exception handling lets a program deal with errors without crashing. When something goes wrong, the program pauses its
normal flow, jumps to code that handles the issue, and then either recovers or exits cleanly.

This library provides the following macros that are used to handle exceptions:

- #THROW
- #TRY
- #CATCH
- #FINALLY

## Throwing Exceptions

Use #THROW to trigger an exception when something goes wrong.

@snippet pet-store.c throw

When we #THROW an exception, the flow of the program moves from the #TRY block to the appropriate #CATCH block. If the
exception is not handled by any of the blocks in the current function, it propagates up the call stack to the function
that called the current function. This continues until the top level of the program is reached. If no block handles the
exception, the program terminates and an error message is printed to the console.

> [!NOTE]
> Error messages can be formatted, just as you would with `printf`. Additionally, if you don't provide an error message,
> the default one for that exception type will be used.

## Trying Risky Code

Use a #TRY block to wrap code that might cause an exception.

@snippet pet-store.c try

These code blocks, by themselves, don't do anything special. But they allow the introduction of other blocks that do
serve specific purposes.

> [!TIP]
> A single #TRY block must be followed by one or more #CATCH blocks to handle the errors, and an optional #FINALLY block
> to execute cleanup code.

## Catching Exceptions

### Handling Specific Types of Exceptions

Use a #CATCH block to handle an exception when it occurs.

@snippet pet-store.c catch

One or more #CATCH blocks can follow a #TRY block. Each #CATCH block must specify the type of exception it handles. If
its type doesn't match the thrown exception, then the block is ignored, and the exception may be caught by the following
blocks.

> [!IMPORTANT]
> When looking for a match, #CATCH blocks are inspected in the order they appear. If you place a generic handler before
> a more specific one, the second block will be unreachable.

### Handling All Kinds of Exceptions

On the other hand, the #CATCH_ALL block is a special block that can handle all types of exceptions.

@snippet pet-store.c catch_all

Only one #CATCH_ALL block is allowed per #TRY block, and it must appear after all type-specific #CATCH blocks if any are
present.

> [!TIP]
> Use #e4c_get_exception to retrieve the exception currently being handled.

## Ensuring Cleanup

A #FINALLY block always runs, no matter whether an exception happens or not.

@snippet pet-store.c finally

This block is optional. And, for each #TRY block, there can be only one #FINALLY block. If an exception occurs, the
#FINALLY block is executed after the #CATCH or block that can handle it. Otherwise, it is executed after the #TRY block.

> [!TIP]
> Use #e4c_is_uncaught to determine whether the thrown exception hasn't been handled yet.


# Advanced Usage

## Dispose Pattern

This is a powerful design pattern for resource management. It is a clean and terse way to handle all kinds of resources
with implicit acquisition and automatic disposal.

These macros save you from having to call the disposing or acquiring functions manually in common situations.

- #USING
- #WITH ... #USE

### Acquiring a Resource Implicitly

A #USING block allows you to automatically acquire and dispose of a resource, by supplying the name of the functions
that must be called. The code block using the resource is free to throw exceptions.

@snippet pet-store.c using

1. The result will be acquired by calling the function with the supplied arguments, if any.
2. The code block will be executed.
3. The result will be disposed of, by calling the other function, no matter whether an exception happens or not.

You can append #CATCH blocks to deal with exceptions that may happen during the manipulation of the resource. Just
remember: by the time the #CATCH block is executed, the resource will already have been disposed of.

@snippet pet-store.c using_catch

> [!TIP]
> You can even append a #FINALLY block for cleanup code other than disposing of the resource.

### Acquiring a Resource Explicitly

Use a #WITH block when the steps to acquire a resource are more complex than simply calling a function. It works exactly
the same as the #USING block, except that you can write the code block in charge of actually acquiring the resource.

@snippet pet-store.c with_use

> [!TIP]
> You can also append #CATCH blocks and an optional #FINALLY block.

## Customization

To customize the way this library behaves you may configure a structure that represents the
[exception context](#e4c_context) of the program.

### Retrieving the Exception Context

Use #e4c_get_context to retrieve the current [exception context](#e4c_context) of the program.

@snippet customization.c get_context

Once you have a reference to this structure, you can set up different handlers.

### Custom Exception Initializer

Exceptions support [custom data](#e4c_exception.data). By default, this data is left uninitialized when an exception is
thrown.

You can set the your own [exception initializer](#e4c_context.initialize_exception) and your function will be executed
whenever an exception is thrown.

@snippet customization.c initialize_exception

> [!TIP]
> For example, you could use this opportunity to capture the entire stacktrace of your program.

### Custom Exception Finalize

You can also set a [exception finalizer](#e4c_context.finalize_exception) to execute your function whenever an exception
is deleted.

@snippet customization.c finalize_exception

> [!TIP]
> This allows you to free any resources you acquired when you initialized an exception's custom data.

### Custom Uncaught Handler

By default, when an exception reaches the top level of the program, it gets printed to the standard error stream.

You can customize this behavior by setting the [uncaught handler](#e4c_context.uncaught_handler) to a custom function
that will be executed in the event of an uncaught exception.

@snippet uncaught-handler.c uncaught_handler

> [!TIP]
> Instead of simply using `stderr`, you could save an error report in a local file.

### Custom Termination Handler

After the uncaught handler has been executed, the program is terminated by calling `exit(EXIT_FAILURE)`.

You can make the library do anything else by setting the [termination handler](#e4c_context.termination_handler) to
execute a function in the event of program termination.

@snippet customization.c termination_handler

> [!TIP]
> In a multithreaded program, you may want to cancel the current thread, instead of terminating the whole program.

### Exception Context Supplier

By default, a predefined exception context is provided and used by the library.

You can create a supplying function and pass it to #e4c_set_context_supplier so you are in full control of the exception
context of your program.

@snippet customization.c set_context_supplier

> [!TIP]
> This mechanism can be useful to provide a concurrent exception handler. For example, your custom context supplier
> could return different instances, depending on which thread is active.

## Multithreading

There is an extension for this library, intended for multithreaded programs.
[exceptions4c-pthreads][EXCEPTIONS4C_PTHREADS] allows you to safely and concurrently use exceptions.

All you have to do is set the exception context supplier, so that each POSIX thread gets its own exception context.

@snippet pthreads.c setup

In the event of an uncaught exception, instead of terminating the program, only the current thread will be canceled.

## Signal Handling

You can turn some standard signals such as `SIGHUP`, `SIGFPE`, and `SIGSEGV` into exceptions, so that they can be caught
and handled in a regular #CATCH block. For example, you could do that to prevent your program from crashing when a null
pointer is dereferenced.

@snippet signals.c null_pointer

However, it's easy to enter undefined behavior territory, due to underspecified behavior and significant implementation
variations regarding signal delivery while a signal handler is executed, so use this technique with caution.

> [!IMPORTANT]
> Keep in mind that the behavior is undefined when `signal` is used in a multithreaded program.


# Additional Info

## Compatibility

exceptions4c rely on modern C features such as `snprintf`, [`bool`][STDBOOL], and [`__VA_OPT__`][__VA_OPT__].

> [!TIP]
> If you need support for older compilers, you can try [exceptions4c-lite][EXCEPTIONS4C_LITE]. It's header-only and
> fully compatible with ANSI C. And if you're looking for a cleaner, safer, and more modern approach to error handling
> that doesn't involve throwing or catching exceptions, you may want to take a look at [Result Library][RESULT_LIBRARY].

## Releases

This library adheres to [Semantic Versioning][SEMVER]. All notable changes for each version are documented in a
[change log][CHANGELOG].

Head over to GitHub for the [latest release][LATEST_RELEASE].

[![Latest Release][BADGE_LATEST_RELEASE]][LATEST_RELEASE]


## Source Code

The source code is [available on GitHub][SOURCE_CODE].

[![Fork me on GitHub][BADGE_GITHUB]][SOURCE_CODE]


[BADGE_GITHUB]:                 https://img.shields.io/badge/Fork_me_on_GitHub-black?logo=github
[BADGE_LATEST_RELEASE]:         https://img.shields.io/github/v/release/guillermocalvo/exceptions4c
[CHANGELOG]:                    https://github.com/guillermocalvo/exceptions4c/blob/main/CHANGELOG.md
[EXAMPLE]:                      example.png
[EXCEPTIONS4C_LITE]:            https://github.com/guillermocalvo/exceptions4c-lite
[EXCEPTIONS4C_PTHREADS]:        https://github.com/guillermocalvo/exceptions4c-pthreads
[LATEST_RELEASE]:               https://github.com/guillermocalvo/exceptions4c/releases/latest
[RESULT_LIBRARY]:               https://result.guillermo.dev/
[SEMVER]:                       https://semver.org/
[SOURCE_CODE]:                  https://github.com/guillermocalvo/exceptions4c
[STDBOOL]:                      https://en.m.wikipedia.org/wiki/C_data_types#stdbool.h
[TYPEOF]:                       https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2899.htm
[__VA_OPT__]:                   https://open-std.org/JTC1/SC22/WG14/www/docs/n3033.htm
