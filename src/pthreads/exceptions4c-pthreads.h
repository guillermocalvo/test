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
 * Thread-safe extension for exceptions4c.
 *
 * <img src="exceptions4c-logo.svg">
 *
 * @file        exceptions4c-pthreads.h
 * @version     1.0.0
 * @author      [Guillermo Calvo](https://guillermo.dev)
 * @copyright   Licensed under Apache 2.0
 * @see         For more information, visit the
 *              [project on GitHub](https://github.com/guillermocalvo/exceptions4c)
 */

#ifndef EXCEPTIONS4C_PTHREADS
#define EXCEPTIONS4C_PTHREADS

#include <exceptions4c.h>

/**
 * Supplies a thread-safe exception context.
 *
 * @return the thread-safe exception context.
 */
struct e4c_context * e4c_pthreads_context_supplier();

#endif
