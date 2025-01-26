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
 * Backtrace extension for exceptions4c.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * @file        exceptions4c-backtrace.h
 * @version     1.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#ifndef EXCEPTIONS4C_BACKTRACE
#define EXCEPTIONS4C_BACKTRACE

#include <exceptions4c.h>

/**
 * Prints an exception and its backtrac.
 *
 * @param exception The uncaught exception.
 */
void e4c_backtrace_uncaught_handler(const struct e4c_exception * exception);

/**
 * Captures the backtrace of a new exception.
 *
 * @param exception The exception being created.
 */
void e4c_backtrace_initialize_exception(struct e4c_exception * exception);

/**
 * Deletes the backtrace of an exception.
 *
 * @param exception The exception being deleted.
 */
void e4c_backtrace_finalize_exception(const struct e4c_exception * exception);

#endif
