cmake_minimum_required(VERSION 3.29)

project(exceptions4c C)

set(CMAKE_C_STANDARD 90)
set(E4C_VERSION "4.0")

add_library(exceptions4c
        src/e4c.c)

add_library(exceptions4c_lite
        lite/src/e4c_lite.c)

include(FetchContent)
FetchContent_Declare(
        doxygen-awesome-css
        URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

# Doxygen
find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
            COMMENT "Generating documentation with Doxygen"
            VERBATIM
    )
else (DOXYGEN_FOUND)
    message("Install Doxygen to generate the documentation")
endif (DOXYGEN_FOUND)

enable_testing()

set(TESTS
        a01
        a02
        a03
        a05
        a06
        a07
        a08
        a09
        a10
        a11
        a12
        a13
        a16
        b01
        b02
        b03
        b04
        b05
        b06
        b07
        b08
        b09
        b10
        b11
        b12
        b14
        b15
        b18
        b19
        b21
        b22
        c01
        c02
        d01
        d02
        d03
        d04
        e01
        e02
        e03
        f01
        f02
        f03
        f04
        f05
        f06
        f07
        g04
        g05
        g06
        g07
        g08
        g09
        g10
        g11
        g12
        i02
        i03
        i04
        i05
        i06
        i07
        i08
        i09
        i10
        i11
        i14
        i15
        i16
        i17
)

foreach(TEST IN LISTS TESTS)
    add_executable(${TEST} "test/${TEST}.c")
    set_target_properties(${TEST} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    target_link_libraries(${TEST} exceptions4c)
    target_include_directories(${TEST} PUBLIC src)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
    set_property(TEST ${TEST} PROPERTY SKIP_RETURN_CODE 77)
endforeach()

set_property(TEST a01 PROPERTY WILL_FAIL TRUE)
set_property(TEST a02 PROPERTY WILL_FAIL TRUE)
set_property(TEST a03 PROPERTY WILL_FAIL TRUE)
set_property(TEST a05 PROPERTY WILL_FAIL TRUE)
set_property(TEST a06 PROPERTY WILL_FAIL TRUE)
set_property(TEST a07 PROPERTY WILL_FAIL TRUE)
set_property(TEST a08 PROPERTY WILL_FAIL TRUE)
set_property(TEST a09 PROPERTY WILL_FAIL TRUE)
set_property(TEST a10 PROPERTY WILL_FAIL TRUE)
set_property(TEST a12 PROPERTY WILL_FAIL TRUE)
set_property(TEST a16 PROPERTY WILL_FAIL TRUE)
set_property(TEST b01 PROPERTY WILL_FAIL TRUE)
set_property(TEST b02 PROPERTY WILL_FAIL TRUE)
set_property(TEST b03 PROPERTY WILL_FAIL TRUE)
set_property(TEST b04 PROPERTY WILL_FAIL TRUE)
set_property(TEST b05 PROPERTY WILL_FAIL TRUE)
set_property(TEST b07 PROPERTY WILL_FAIL TRUE)
set_property(TEST b08 PROPERTY WILL_FAIL TRUE)
set_property(TEST b09 PROPERTY WILL_FAIL TRUE)
set_property(TEST b10 PROPERTY WILL_FAIL TRUE)
set_property(TEST b12 PROPERTY WILL_FAIL TRUE)
set_property(TEST b15 PROPERTY WILL_FAIL TRUE)
set_property(TEST b18 PROPERTY WILL_FAIL TRUE)
set_property(TEST b19 PROPERTY WILL_FAIL TRUE)
set_property(TEST b21 PROPERTY WILL_FAIL TRUE)
set_property(TEST c01 PROPERTY WILL_FAIL TRUE)
set_property(TEST c02 PROPERTY WILL_FAIL TRUE)
set_property(TEST d01 PROPERTY WILL_FAIL TRUE)
set_property(TEST d02 PROPERTY WILL_FAIL TRUE)
set_property(TEST d03 PROPERTY WILL_FAIL TRUE)
set_property(TEST d04 PROPERTY WILL_FAIL TRUE)
set_property(TEST g04 PROPERTY WILL_FAIL TRUE)
set_property(TEST g05 PROPERTY WILL_FAIL TRUE)
set_property(TEST g06 PROPERTY WILL_FAIL TRUE)
set_property(TEST i05 PROPERTY WILL_FAIL TRUE)

set(TESTS
        uncaught
        caught
        cleanup
        hierarchy
        limits
        overflow
)

foreach(TEST IN LISTS TESTS)
    add_executable(${TEST} "lite/test/${TEST}.c")
    set_target_properties(${TEST} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    target_link_libraries(${TEST} exceptions4c_lite)
    target_include_directories(${TEST} PUBLIC lite/src)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
    set_property(TEST ${TEST} PROPERTY SKIP_RETURN_CODE 77)
endforeach()

set_property(TEST uncaught PROPERTY WILL_FAIL TRUE)
set_property(TEST overflow PROPERTY WILL_FAIL TRUE)
