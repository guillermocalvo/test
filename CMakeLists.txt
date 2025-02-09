cmake_minimum_required(VERSION 3.29)

project(exceptions4c C)

set(CMAKE_C_STANDARD 90)
set(EXCEPTIONS4C_VERSION "4.0")

add_library(exceptions4c
        src/exceptions4c.c)

target_include_directories(exceptions4c PUBLIC src)

add_library(exceptions4c_pthreads
        src/pthreads/exceptions4c-pthreads.c)

target_include_directories(exceptions4c_pthreads PUBLIC src src/pthreads)

include(FetchContent)
FetchContent_Declare(
        doxygen-awesome-css
        URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(doxygen-awesome-css)
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

# POSIX Threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

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
        get-exception
        is-uncaught
        catch-all
        uncaught-exception-1
        uncaught-exception-2
        e01
        e02
        e03
        catch-specific-exception
        f02
        f03
        f04
        f05
        f06
        f07
        catch-sigsegv
        catch-sigterm
        catch-sigint
        retry
        reacquire
        throw-with-format
        uncaught-handler
        initialize-exception
        finalize-exception
        exception-cause
)

foreach(TEST IN LISTS TESTS)
    add_executable(${TEST} "tests/${TEST}.c")
    set_target_properties(${TEST} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    target_link_libraries(${TEST} exceptions4c)
    target_include_directories(${TEST} PUBLIC src)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
    set_property(TEST ${TEST} PROPERTY SKIP_RETURN_CODE 77)
endforeach()

set_property(TEST uncaught-exception-1 PROPERTY WILL_FAIL TRUE)
set_property(TEST uncaught-exception-2 PROPERTY WILL_FAIL TRUE)

set(TESTS
        uncaught
        caught
        catch-all
        cleanup
        limits
)

foreach(TEST IN LISTS TESTS)
    add_executable(lite-${TEST} "tests/lite/${TEST}.c")
    set_target_properties(lite-${TEST} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    target_include_directories(lite-${TEST} PUBLIC src/lite)
    add_test(NAME lite-${TEST} COMMAND $<TARGET_FILE:lite-${TEST}>)
    set_property(TEST lite-${TEST} PROPERTY SKIP_RETURN_CODE 77)
endforeach()

set_property(TEST lite-uncaught PROPERTY WILL_FAIL TRUE)

set(TESTS
        multithread
)

foreach(TEST IN LISTS TESTS)
    add_executable(pthreads-${TEST} "tests/pthreads/${TEST}.c")
    set_target_properties(pthreads-${TEST} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    target_link_libraries(pthreads-${TEST}
            exceptions4c
            exceptions4c_pthreads
            Threads::Threads
    )
    target_include_directories(pthreads-${TEST} PUBLIC src src/pthreads)
    add_test(NAME pthreads-${TEST} COMMAND $<TARGET_FILE:pthreads-${TEST}>)
    set_property(TEST pthreads-${TEST} PROPERTY SKIP_RETURN_CODE 77)
endforeach()

# EXAMPLES

add_executable(examples-pthreads "examples/pthreads.c")
target_link_libraries(examples-pthreads exceptions4c Threads::Threads)
target_include_directories(examples-pthreads PUBLIC src examples)
add_test(NAME examples-pthreads COMMAND $<TARGET_FILE:examples-pthreads>)

add_executable(examples-signals "examples/signals.c")
target_link_libraries(examples-signals exceptions4c)
target_include_directories(examples-signals PUBLIC src)
add_test(NAME examples-signals COMMAND $<TARGET_FILE:examples-signals>)

add_executable(examples-pet-store "examples/pet-store.c")
target_link_libraries(examples-pet-store exceptions4c)
target_include_directories(examples-pet-store PUBLIC src)
add_test(NAME examples-pet-store COMMAND $<TARGET_FILE:examples-pet-store>)

add_executable(examples-customization "examples/customization.c")
target_link_libraries(examples-customization exceptions4c)
target_include_directories(examples-customization PUBLIC src)
add_test(NAME examples-customization COMMAND $<TARGET_FILE:examples-customization>)

add_executable(examples-uncaught-handler "examples/uncaught-handler.c")
target_link_libraries(examples-uncaught-handler exceptions4c)
target_include_directories(examples-uncaught-handler PUBLIC src)
add_test(NAME examples-uncaught-handler COMMAND $<TARGET_FILE:examples-uncaught-handler>)
set_property(TEST examples-uncaught-handler PROPERTY WILL_FAIL TRUE)
