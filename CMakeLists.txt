cmake_minimum_required(VERSION 3.29)

project(exceptions4c C)

set(CMAKE_C_STANDARD 90)
set(EXCEPTIONS4C_VERSION "4.0")

add_library(exceptions4c
        src/exceptions4c.c)

add_library(exceptions4c_lite
        src/lite/exceptions4c-lite.c)

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
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
            COMMENT "Generating documentation with Doxygen"
            VERBATIM
    )
else (DOXYGEN_FOUND)
    message("Install Doxygen to generate the documentation")
endif (DOXYGEN_FOUND)

enable_testing()

set(TESTS
        a11
        a12
        b11
        b15
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
        i07
        i08
        i09
        i10
        i11
        i14
        i15
        i17
)

foreach(TEST IN LISTS TESTS)
    add_executable(${TEST} "tests/${TEST}.c")
    set_target_properties(${TEST} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    target_link_libraries(${TEST} exceptions4c)
    target_include_directories(${TEST} PUBLIC src)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
    set_property(TEST ${TEST} PROPERTY SKIP_RETURN_CODE 77)
endforeach()

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
        catch-all
        cleanup
        hierarchy
        limits
        overflow
)

foreach(TEST IN LISTS TESTS)
    add_executable(${TEST} "tests/lite/${TEST}.c")
    set_target_properties(${TEST} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    target_link_libraries(${TEST} exceptions4c_lite)
    target_include_directories(${TEST} PUBLIC src/lite)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
    set_property(TEST ${TEST} PROPERTY SKIP_RETURN_CODE 77)
endforeach()

set_property(TEST uncaught PROPERTY WILL_FAIL TRUE)
set_property(TEST overflow PROPERTY WILL_FAIL TRUE)
