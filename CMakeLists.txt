cmake_minimum_required(VERSION 3.29)

project(exceptions4c C)

set(CMAKE_C_STANDARD 99)
set(EXCEPTIONS4C_VERSION "4.0")

add_library(exceptions4c
        src/exceptions4c.c)

target_include_directories(exceptions4c PUBLIC src)

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
        panic-try
        panic-retry
        panic-reacquire
        panic-context
        panic-block-try
        panic-block-catch
        panic-block-next
        panic-dangling
        get-exception
        is-uncaught
        catch-all
        throw-uncaught-1
        throw-uncaught-2
        catch-specific
        catch-generic
        catch-duplicate
        catch-unordered
        finally
        throw-suppressed
        catch-sigsegv
        catch-sigterm
        catch-sigint
        retry
        reacquire
        throw-format
        handler-uncaught
        handler-initialize
        handler-finalize
        throw-cause
        with-use
)

foreach(TEST IN LISTS TESTS)
    add_executable(${TEST} "tests/${TEST}.c")
    set_target_properties(${TEST} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    target_link_libraries(${TEST} exceptions4c)
    target_include_directories(${TEST} PUBLIC src)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
endforeach()

set_property(TEST panic-try PROPERTY WILL_FAIL TRUE)
set_property(TEST panic-retry PROPERTY WILL_FAIL TRUE)
set_property(TEST panic-reacquire PROPERTY WILL_FAIL TRUE)
set_property(TEST panic-context PROPERTY WILL_FAIL TRUE)
set_property(TEST panic-block-try PROPERTY WILL_FAIL TRUE)
set_property(TEST panic-block-catch PROPERTY WILL_FAIL TRUE)
set_property(TEST panic-block-next PROPERTY WILL_FAIL TRUE)
set_property(TEST panic-dangling PROPERTY WILL_FAIL TRUE)
set_property(TEST throw-uncaught-1 PROPERTY WILL_FAIL TRUE)
set_property(TEST throw-uncaught-2 PROPERTY WILL_FAIL TRUE)

# EXAMPLES

add_executable(examples-pthreads "examples/pthreads.c")
target_link_libraries(examples-pthreads exceptions4c Threads::Threads)
target_include_directories(examples-pthreads PUBLIC src)
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
