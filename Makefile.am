#
# exceptions4c
#
# Copyright (c) 2025 Guillermo Calvo
# Licensed under the GNU Lesser General Public License
#

AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CFLAGS = -Wall -Werror --pedantic -Wno-missing-braces -Wno-dangling-else -coverage -O0 -I$(EXCEPTIONS4C_PATH) -Isrc/lite -I$(EXCEPTIONS4C_PTHREADS_PATH) -I$(EXCEPTIONS4C_BACKTRACE_PATH)


# Library

EXCEPTIONS4C_PATH           = src
EXCEPTIONS4C_LIBRARY        = lib/libexceptions4c.a
EXCEPTIONS4C_PTHREADS_PATH  = src/pthreads
EXCEPTIONS4C_PTHREADS_LIBRARY = lib/libexceptions4c-pthreads.a
EXCEPTIONS4C_BACKTRACE_PATH = src/backtrace
EXCEPTIONS4C_BACKTRACE_LIBRARY = lib/libexceptions4c-backtrace.a


# Install

lib_LIBRARIES =                         \
    $(EXCEPTIONS4C_LIBRARY)             \
    $(EXCEPTIONS4C_PTHREADS_LIBRARY)    \
    $(EXCEPTIONS4C_BACKTRACE_LIBRARY)

include_HEADERS =                       \
    src/exceptions4c.h                  \
    src/lite/exceptions4c-lite.h        \
    src/pthreads/exceptions4c-pthreads.h \
    src/backtrace/exceptions4c-backtrace.h


# Documentation

docsdir = $(datadir)/docs/exceptions4c
docs_DATA = docs/*


# Cleanup

CLEANFILES =                            \
    *.log                               \
    *.gcda                              \
    *.gcno                              \
    *.gcov                              \
    *.stackdump                         \
    src/*.gcda                          \
    src/*.gcno                          \
    src/*.gcov                          \
    src/pthreads/*.gcda                 \
    src/pthreads/*.gcno                 \
    src/pthreads/*.gcov                 \
    src/backtrace/*.gcda                \
    src/backtrace/*.gcno                \
    src/backtrace/*.gcov                \
    tests/*.gcda                        \
    tests/*.gcno                        \
    tests/*.gcov                        \
    tests/lite/*.gcda                   \
    tests/lite/*.gcno                   \
    tests/lite/*.gcov                   \
    tests/pthreads/*.gcda               \
    tests/pthreads/*.gcno               \
    tests/pthreads/*.gcov               \
    tests/backtrace/*.gcda              \
    tests/backtrace/*.gcno              \
    tests/backtrace/*.gcov


# Check

check_PROGRAMS =                        \
    bin/check/a10                       \
    bin/check/get-exception             \
    bin/check/is-uncaught               \
    bin/check/b08                       \
    bin/check/b10                       \
    bin/check/catch-all                 \
    bin/check/b18                       \
    bin/check/b19                       \
    bin/check/b22                       \
    bin/check/uncaught-exception-1      \
    bin/check/uncaught-exception-2      \
    bin/check/e01                       \
    bin/check/e02                       \
    bin/check/e03                       \
    bin/check/catch-specific-exception  \
    bin/check/catch-generic-exception   \
    bin/check/f03                       \
    bin/check/f04                       \
    bin/check/f05                       \
    bin/check/f06                       \
    bin/check/f07                       \
    bin/check/catch-sigsegv             \
    bin/check/catch-sigterm             \
    bin/check/catch-sigint              \
    bin/check/retry                     \
    bin/check/reacquire                 \
    bin/check/throw-with-format         \
    bin/check/uncaught-handler          \
    bin/check/initialize-exception      \
    bin/check/finalize-exception        \
    bin/check/exception-cause           \
    bin/check/examples/pthreads         \
    bin/check/examples/signals          \
    bin/check/examples/pet-store        \
    bin/check/examples/customization    \
    bin/check/examples/uncaught-handler \
    bin/check/lite/uncaught             \
    bin/check/lite/caught               \
    bin/check/lite/catch-all            \
    bin/check/lite/cleanup              \
    bin/check/lite/limits               \
    bin/check/lite/overflow             \
    bin/check/pthreads/multithread      \
    bin/check/pthreads/dangling-context \
    bin/check/backtrace/backtraced

TESTS =                                 \
    bin/check/a10                       \
    bin/check/get-exception             \
    bin/check/is-uncaught               \
    bin/check/b08                       \
    bin/check/b10                       \
    bin/check/catch-all                 \
    bin/check/b18                       \
    bin/check/b19                       \
    bin/check/b22                       \
    bin/check/uncaught-exception-1      \
    bin/check/uncaught-exception-2      \
    bin/check/e01                       \
    bin/check/e02                       \
    bin/check/e03                       \
    bin/check/catch-specific-exception  \
    bin/check/catch-generic-exception   \
    bin/check/f03                       \
    bin/check/f04                       \
    bin/check/f05                       \
    bin/check/f06                       \
    bin/check/f07                       \
    bin/check/catch-sigsegv             \
    bin/check/catch-sigterm             \
    bin/check/catch-sigint              \
    bin/check/retry                     \
    bin/check/reacquire                 \
    bin/check/throw-with-format         \
    bin/check/uncaught-handler          \
    bin/check/initialize-exception      \
    bin/check/finalize-exception        \
    bin/check/exception-cause           \
    bin/check/examples/pthreads         \
    bin/check/examples/signals          \
    bin/check/examples/pet-store        \
    bin/check/examples/customization    \
    bin/check/examples/uncaught-handler \
    bin/check/lite/uncaught             \
    bin/check/lite/caught               \
    bin/check/lite/catch-all            \
    bin/check/lite/cleanup              \
    bin/check/lite/limits               \
    bin/check/lite/overflow             \
    bin/check/pthreads/multithread      \
    bin/check/pthreads/dangling-context \
    bin/check/backtrace/backtraced

XFAIL_TESTS =                           \
    bin/check/a10                       \
    bin/check/b08                       \
    bin/check/b10                       \
    bin/check/b18                       \
    bin/check/b19                       \
    bin/check/b22                       \
    bin/check/uncaught-exception-1      \
    bin/check/uncaught-exception-2      \
    bin/check/examples/uncaught-handler \
    bin/check/lite/uncaught             \
    bin/check/lite/overflow             \
    bin/check/backtrace/backtraced

tests: check


# exceptions4c library

lib_libexceptions4c_a_CFLAGS            = -Wall -Werror --pedantic -Wno-missing-braces -I$(EXCEPTIONS4C_PATH)
lib_libexceptions4c_a_SOURCES           = src/exceptions4c.c


# exceptions4c-pthreads library

lib_libexceptions4c_pthreads_a_CFLAGS   = -Wall -Werror --pedantic -Wno-missing-braces -I$(EXCEPTIONS4C_PATH) -I$(EXCEPTIONS4C_PTHREADS_PATH)
lib_libexceptions4c_pthreads_a_SOURCES  = src/pthreads/exceptions4c-pthreads.c


# exceptions4c-backtrace library

lib_libexceptions4c_backtrace_a_CFLAGS  = -Wall -Werror --pedantic -Wno-missing-braces -I$(EXCEPTIONS4C_PATH) -I$(EXCEPTIONS4C_BACKTRACE_PATH)
lib_libexceptions4c_backtrace_a_SOURCES = src/backtrace/exceptions4c-backtrace.c


# exceptions4c tests

bin_check_a10_SOURCES                   = src/exceptions4c.c tests/a10.c
bin_check_get_exception_SOURCES         = src/exceptions4c.c tests/get-exception.c
bin_check_is_uncaught_SOURCES           = src/exceptions4c.c tests/is-uncaught.c
bin_check_b08_SOURCES                   = src/exceptions4c.c tests/b08.c
bin_check_b10_SOURCES                   = src/exceptions4c.c tests/b10.c
bin_check_catch_all_SOURCES             = src/exceptions4c.c tests/catch-all.c
bin_check_b18_SOURCES                   = src/exceptions4c.c tests/b18.c
bin_check_b19_SOURCES                   = src/exceptions4c.c tests/b19.c
bin_check_b22_SOURCES                   = src/exceptions4c.c tests/b22.c
bin_check_uncaught_exception_1_SOURCES  = src/exceptions4c.c tests/uncaught-exception-1.c
bin_check_uncaught_exception_2_SOURCES  = src/exceptions4c.c tests/uncaught-exception-2.c
bin_check_e01_SOURCES                   = src/exceptions4c.c tests/e01.c
bin_check_e02_SOURCES                   = src/exceptions4c.c tests/e02.c
bin_check_e03_SOURCES                   = src/exceptions4c.c tests/e03.c
bin_check_catch_specific_exception_SOURCES = src/exceptions4c.c tests/catch-specific-exception.c
bin_check_catch_generic_exception_SOURCES = src/exceptions4c.c tests/catch-generic-exception.c
bin_check_f03_SOURCES                   = src/exceptions4c.c tests/f03.c
bin_check_f04_SOURCES                   = src/exceptions4c.c tests/f04.c
bin_check_f05_SOURCES                   = src/exceptions4c.c tests/f05.c
bin_check_f06_SOURCES                   = src/exceptions4c.c tests/f06.c
bin_check_f07_SOURCES                   = src/exceptions4c.c tests/f07.c
bin_check_catch_sigsegv_SOURCES         = src/exceptions4c.c tests/catch-sigsegv.c
bin_check_catch_sigterm_SOURCES         = src/exceptions4c.c tests/catch-sigterm.c
bin_check_catch_sigint_SOURCES          = src/exceptions4c.c tests/catch-sigint.c
bin_check_retry_SOURCES                 = src/exceptions4c.c tests/retry.c
bin_check_reacquire_SOURCES             = src/exceptions4c.c tests/reacquire.c
bin_check_throw_with_format_SOURCES     = src/exceptions4c.c tests/throw-with-format.c
bin_check_uncaught_handler_SOURCES      = src/exceptions4c.c tests/uncaught-handler.c
bin_check_initialize_exception_SOURCES  = src/exceptions4c.c tests/initialize-exception.c
bin_check_finalize_exception_SOURCES    = src/exceptions4c.c tests/finalize-exception.c
bin_check_exception_cause_SOURCES       = src/exceptions4c.c tests/exception-cause.c

# exceptions4c examples

bin_check_examples_pthreads_CFLAGS      = -Wall -Werror --pedantic -Wno-missing-braces -I$(EXCEPTIONS4C_PATH) -Iexamples
bin_check_examples_pthreads_SOURCES     = src/exceptions4c.c examples/pthreads.c
bin_check_examples_signals_SOURCES      = src/exceptions4c.c examples/signals.c
bin_check_examples_pet_store_SOURCES    = src/exceptions4c.c examples/pet-store.c
bin_check_examples_customization_SOURCES = src/exceptions4c.c examples/customization.c
bin_check_examples_uncaught_handler_SOURCES = src/exceptions4c.c examples/uncaught-handler.c


# exceptions4c tests (lightweight version)

bin_check_lite_uncaught_SOURCES         = tests/lite/uncaught.c
bin_check_lite_caught_SOURCES           = tests/lite/caught.c
bin_check_lite_catch_all_SOURCES        = tests/lite/catch-all.c
bin_check_lite_cleanup_SOURCES          = tests/lite/cleanup.c
bin_check_lite_limits_SOURCES           = tests/lite/limits.c
bin_check_lite_overflow_SOURCES         = tests/lite/overflow.c


# exceptions4c-pthreads tests

bin_check_pthreads_multithread_SOURCES  = src/exceptions4c.c src/pthreads/exceptions4c-pthreads.c tests/pthreads/multithread.c
bin_check_pthreads_dangling_context_SOURCES = src/exceptions4c.c src/pthreads/exceptions4c-pthreads.c tests/pthreads/dangling-context.c


# exceptions4c-backtrace tests

bin_check_backtrace_backtraced_SOURCES  = src/exceptions4c.c src/backtrace/exceptions4c-backtrace.c tests/backtrace/backtraced.c


# coverage

coverage: exceptions4c.c.gcov exceptions4c-pthreads.c.gcov exceptions4c-backtrace.c.gcov

coverage-report: docs/html/coverage/index.html

docs/html/coverage/index.html: coverage.info
	genhtml coverage.info --output-directory docs/html/coverage

coverage.info: exceptions4c.c.gcov exceptions4c-pthreads.c.gcov exceptions4c-backtrace.c.gcov
	lcov --ignore-errors mismatch --capture --directory . --output-file coverage.info

exceptions4c.c.gcov: src/exceptions4c.gcda
	gcov --verbose src/exceptions4c.c

exceptions4c-pthreads.c.gcov: src/pthreads/exceptions4c-pthreads.gcda
	gcov --verbose src/pthreads/exceptions4c-pthreads.c

exceptions4c-backtrace.c.gcov: src/backtrace/exceptions4c-backtrace.gcda
	gcov --verbose src/backtrace/exceptions4c-backtrace.c

src/exceptions4c.gcda: check

src/pthreads/exceptions4c-pthreads.gcda: check

src/backtrace/exceptions4c-backtrace.gcda: check

# generate documentation

docs: docs/html/index.html

docs/html/index.html: docs/Doxyfile docs/doxygen-awesome.css docs/doxygen-awesome-sidebar-only.css
	doxygen docs/Doxyfile

docs/doxygen-awesome.css:
	wget --verbose --output-document docs/doxygen-awesome.css https://github.com/jothepro/doxygen-awesome-css/raw/refs/heads/main/doxygen-awesome.css

docs/doxygen-awesome-sidebar-only.css:
	wget --verbose --output-document docs/doxygen-awesome-sidebar-only.css https://github.com/jothepro/doxygen-awesome-css/raw/refs/heads/main/doxygen-awesome-sidebar-only.css
