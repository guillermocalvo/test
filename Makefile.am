#
# exceptions4c
#
# Copyright (c) 2025 Guillermo Calvo
# Licensed under the GNU Lesser General Public License
#

AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CFLAGS = -Wall -Werror --pedantic -Wno-missing-braces -Wno-dangling-else -coverage -O0 -I$(EXCEPTIONS4C_PATH) -I$(EXCEPTIONS4C_LITE_PATH)


# Library

EXCEPTIONS4C_PATH           = src
EXCEPTIONS4C_LIBRARY        = lib/libexceptions4c.a

EXCEPTIONS4C_LITE_PATH      = lite/src
EXCEPTIONS4C_LITE_LIBRARY   = lib/libexceptions4c_lite.a


# Install

lib_LIBRARIES =                         \
    $(EXCEPTIONS4C_LIBRARY)             \
    $(EXCEPTIONS4C_LITE_LIBRARY)

include_HEADERS =                       \
    src/exceptions4c.h                  \
    lite/src/exceptions4c-lite.h


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
    tests/*.gcda                         \
    tests/*.gcno                         \
    tests/*.gcov                         \
    lite/src/*.gcda                     \
    lite/src/*.gcno                     \
    lite/src/*.gcov                     \
    lite/tests/*.gcda                    \
    lite/tests/*.gcno                    \
    lite/tests/*.gcov


# Check

check_PROGRAMS =                        \
	bin/check/a10                       \
	bin/check/a11                       \
	bin/check/a12                       \
	bin/check/b08                       \
	bin/check/b10                       \
	bin/check/b11                       \
	bin/check/b15                       \
	bin/check/b18                       \
	bin/check/b19                       \
	bin/check/b22                       \
	bin/check/d01                       \
	bin/check/d02                       \
	bin/check/d03                       \
	bin/check/d04                       \
	bin/check/e01                       \
	bin/check/e02                       \
	bin/check/e03                       \
	bin/check/f01                       \
	bin/check/f02                       \
	bin/check/f03                       \
	bin/check/f04                       \
	bin/check/f05                       \
	bin/check/f06                       \
	bin/check/f07                       \
	bin/check/g04                       \
	bin/check/g05                       \
	bin/check/g06                       \
	bin/check/g07                       \
	bin/check/g08                       \
	bin/check/g09                       \
	bin/check/g10                       \
	bin/check/g11                       \
	bin/check/g12                       \
	bin/check/i02                       \
	bin/check/i03                       \
	bin/check/i04                       \
	bin/check/i05                       \
	bin/check/i07                       \
	bin/check/i08                       \
	bin/check/i09                       \
	bin/check/i10                       \
	bin/check/i11                       \
	bin/check/i14                       \
	bin/check/i15                       \
	bin/check/i17                       \
    bin/check/lite/uncaught             \
    bin/check/lite/caught               \
    bin/check/lite/cleanup              \
    bin/check/lite/hierarchy            \
    bin/check/lite/limits               \
    bin/check/lite/overflow

TESTS =                                 \
	bin/check/a10                       \
	bin/check/a11                       \
	bin/check/a12                       \
	bin/check/b08                       \
	bin/check/b10                       \
	bin/check/b11                       \
	bin/check/b15                       \
	bin/check/b18                       \
	bin/check/b19                       \
	bin/check/b22                       \
	bin/check/d01                       \
	bin/check/d02                       \
	bin/check/d03                       \
	bin/check/d04                       \
	bin/check/e01                       \
	bin/check/e02                       \
	bin/check/e03                       \
	bin/check/f01                       \
	bin/check/f02                       \
	bin/check/f03                       \
	bin/check/f04                       \
	bin/check/f05                       \
	bin/check/f06                       \
	bin/check/f07                       \
	bin/check/g04                       \
	bin/check/g05                       \
	bin/check/g06                       \
	bin/check/g07                       \
	bin/check/g08                       \
	bin/check/g09                       \
	bin/check/g10                       \
	bin/check/g11                       \
	bin/check/g12                       \
	bin/check/i02                       \
	bin/check/i03                       \
	bin/check/i04                       \
	bin/check/i05                       \
	bin/check/i07                       \
	bin/check/i08                       \
	bin/check/i09                       \
	bin/check/i10                       \
	bin/check/i11                       \
	bin/check/i14                       \
	bin/check/i15                       \
	bin/check/i17                       \
    bin/check/lite/uncaught             \
    bin/check/lite/caught               \
    bin/check/lite/cleanup              \
    bin/check/lite/hierarchy            \
    bin/check/lite/limits               \
    bin/check/lite/overflow

XFAIL_TESTS =                           \
	bin/check/a10                       \
	bin/check/b08                       \
	bin/check/b10                       \
	bin/check/b18                       \
	bin/check/b19                       \
	bin/check/b22                       \
	bin/check/d01                       \
	bin/check/d02                       \
	bin/check/d03                       \
	bin/check/d04                       \
	bin/check/g04                       \
	bin/check/g05                       \
	bin/check/g06                       \
	bin/check/i05                       \
    bin/check/lite/uncaught             \
    bin/check/lite/overflow

tests: check


# exceptions4c library

lib_libexceptions4c_a_CFLAGS            = -Wall -Werror --pedantic -Wno-missing-braces
lib_libexceptions4c_a_SOURCES           = src/exceptions4c.c


# exceptions4c library (lightweight version)

lib_libexceptions4c_lite_a_CFLAGS       = -Wall -Werror --pedantic -Wno-missing-braces
lib_libexceptions4c_lite_a_SOURCES      = lite/src/exceptions4c-lite.c


# exceptions4c tests

bin_check_a10_SOURCES                   = src/exceptions4c.c tests/a10.c
bin_check_a11_SOURCES                   = src/exceptions4c.c tests/a11.c
bin_check_a12_SOURCES                   = src/exceptions4c.c tests/a12.c
bin_check_b08_SOURCES                   = src/exceptions4c.c tests/b08.c
bin_check_b10_SOURCES                   = src/exceptions4c.c tests/b10.c
bin_check_b11_SOURCES                   = src/exceptions4c.c tests/b11.c
bin_check_b15_SOURCES                   = src/exceptions4c.c tests/b15.c
bin_check_b18_SOURCES                   = src/exceptions4c.c tests/b18.c
bin_check_b19_SOURCES                   = src/exceptions4c.c tests/b19.c
bin_check_b22_SOURCES                   = src/exceptions4c.c tests/b22.c
bin_check_d01_SOURCES                   = src/exceptions4c.c tests/d01.c
bin_check_d02_SOURCES                   = src/exceptions4c.c tests/d02.c
bin_check_d03_SOURCES                   = src/exceptions4c.c tests/d03.c
bin_check_d04_SOURCES                   = src/exceptions4c.c tests/d04.c
bin_check_e01_SOURCES                   = src/exceptions4c.c tests/e01.c
bin_check_e02_SOURCES                   = src/exceptions4c.c tests/e02.c
bin_check_e03_SOURCES                   = src/exceptions4c.c tests/e03.c
bin_check_f01_SOURCES                   = src/exceptions4c.c tests/f01.c
bin_check_f02_SOURCES                   = src/exceptions4c.c tests/f02.c
bin_check_f03_SOURCES                   = src/exceptions4c.c tests/f03.c
bin_check_f04_SOURCES                   = src/exceptions4c.c tests/f04.c
bin_check_f05_SOURCES                   = src/exceptions4c.c tests/f05.c
bin_check_f06_SOURCES                   = src/exceptions4c.c tests/f06.c
bin_check_f07_SOURCES                   = src/exceptions4c.c tests/f07.c
bin_check_g04_SOURCES                   = src/exceptions4c.c tests/g04.c
bin_check_g05_SOURCES                   = src/exceptions4c.c tests/g05.c
bin_check_g06_SOURCES                   = src/exceptions4c.c tests/g06.c
bin_check_g07_SOURCES                   = src/exceptions4c.c tests/g07.c
bin_check_g08_SOURCES                   = src/exceptions4c.c tests/g08.c
bin_check_g09_SOURCES                   = src/exceptions4c.c tests/g09.c
bin_check_g10_SOURCES                   = src/exceptions4c.c tests/g10.c
bin_check_g11_SOURCES                   = src/exceptions4c.c tests/g11.c
bin_check_g12_SOURCES                   = src/exceptions4c.c tests/g12.c
bin_check_i02_SOURCES                   = src/exceptions4c.c tests/i02.c
bin_check_i03_SOURCES                   = src/exceptions4c.c tests/i03.c
bin_check_i04_SOURCES                   = src/exceptions4c.c tests/i04.c
bin_check_i05_SOURCES                   = src/exceptions4c.c tests/i05.c
bin_check_i07_SOURCES                   = src/exceptions4c.c tests/i07.c
bin_check_i08_SOURCES                   = src/exceptions4c.c tests/i08.c
bin_check_i09_SOURCES                   = src/exceptions4c.c tests/i09.c
bin_check_i10_SOURCES                   = src/exceptions4c.c tests/i10.c
bin_check_i11_SOURCES                   = src/exceptions4c.c tests/i11.c
bin_check_i14_SOURCES                   = src/exceptions4c.c tests/i14.c
bin_check_i15_SOURCES                   = src/exceptions4c.c tests/i15.c
bin_check_i17_SOURCES                   = src/exceptions4c.c tests/i17.c


# exceptions4c tests (lightweight version)

bin_check_lite_uncaught_SOURCES         = lite/src/exceptions4c-lite.c lite/test/uncaught.c
bin_check_lite_caught_SOURCES           = lite/src/exceptions4c-lite.c lite/test/caught.c
bin_check_lite_cleanup_SOURCES          = lite/src/exceptions4c-lite.c lite/test/cleanup.c
bin_check_lite_hierarchy_SOURCES        = lite/src/exceptions4c-lite.c lite/test/hierarchy.c
bin_check_lite_limits_SOURCES           = lite/src/exceptions4c-lite.c lite/test/limits.c
bin_check_lite_overflow_SOURCES         = lite/src/exceptions4c-lite.c lite/test/overflow.c

# coverage

coverage: check exceptions4c.c.gcov exceptions4c-lite.c.gcov

exceptions4c.c.gcov:
	gcov src/exceptions4c.c --object-directory src/*

exceptions4c_lite.c.gcov:
	gcov lite/src/exceptions4c-lite.c --object-directory lite/src/*

# generate documentation

docs: docs/html/index.html

docs/html/index.html:
	doxygen ./etc/Doxyfile
