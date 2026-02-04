cmake_minimum_required(VERSION 3.16)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build Type" FORCE)
endif()

project(gb
    VERSION 1.0
    DESCRIPTION "Architectural Limits of Interpreters"
    LANGUAGES C)

find_package(Threads REQUIRED)

add_library(release-flags INTERFACE)
target_compile_options(release-flags
    INTERFACE
    -O3
    -Wall
    -Wextra
    -Wfloat-equal
    -Wundef
    -Wshadow
    -Wpointer-arith
    -Wcast-align
    -Wstrict-prototypes
    -Wstrict-overflow
    -Wno-unused-variable
    -Wno-unused-parameter
    -march=native)
add_library(debug-flags INTERFACE)
target_compile_options(debug-flags
    INTERFACE
    -g
    -Wall
    -Wextra
    -Wfloat-equal
    -Wundef
    -Wshadow
    -Wpointer-arith
    -Wcast-align
    -Wstrict-overflow
    -Wno-unused-variable
    -Wno-unused-parameter
    -fsanitize=address
    -fno-omit-frame-pointer
    -march=native)
target_link_options(debug-flags
    INTERFACE
    -fsanitize=address)

#### core
add_library(supporting
    ${CMAKE_CURRENT_SOURCE_DIR}/loader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log.c)
target_link_libraries(supporting
    PRIVATE
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)

############################# TARGET ##################################
add_executable(switch
    ${CMAKE_CURRENT_SOURCE_DIR}/switch.c)
add_executable(computed-goto
    ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c)
add_executable(decoupled
    ${CMAKE_CURRENT_SOURCE_DIR}/decoupled.c)
add_executable(testsuite
    ${CMAKE_CURRENT_SOURCE_DIR}/testsuite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/switch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decoupled.c)

target_compile_definitions(switch
    PRIVATE
    __SWITCH_MAIN__)
target_compile_definitions(computed-goto
    PRIVATE
    __COMPUTED_GOTO_MAIN__)
target_compile_definitions(decoupled
    PRIVATE
    __DECOUPLED_MAIN__)
target_compile_definitions(testsuite
    PRIVATE
    __TESTSUITE_MAIN__)

target_include_directories(switch
    PRIVATE
    .
    templates)
target_include_directories(computed-goto
    PRIVATE
    .
    templates)
target_include_directories(decoupled
    PRIVATE
    .
    templates)
target_include_directories(testsuite
    PRIVATE
    .
    tests
    templates)

target_link_libraries(switch
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(computed-goto
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(decoupled
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(testsuite
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
