cmake_minimum_required(VERSION 3.16)

include(ExternalProject)

project(dissertation
    VERSION 1.0
    DESCRIPTION "Architectural Limits of Interpreters"
    LANGUAGES C)

find_package(Threads REQUIRED)

############################# TOOLCHAIN ##################################
ExternalProject_Add(toolchain
    GIT_REPOSITORY    https://github.com/richfelker/musl-cross-make.git
    GIT_TAG           master
    SOURCE_DIR        "${CMAKE_BINARY_DIR}/musl-cross-make"

    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy
                      ${CMAKE_CURRENT_SOURCE_DIR}/toolchain_config.mak
                      <SOURCE_DIR>/config.mak

    BUILD_COMMAND     make -j4
    INSTALL_COMMAND   make install

    BUILD_IN_SOURCE   1)

############################# SAMPLES ##################################
ExternalProject_Add(samples
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/samples"
    BINARY_DIR "${CMAKE_BINARY_DIR}/samples"
    CMAKE_ARGS 
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/samples.toolchain.cmake
    DEPENDS toolchain)

############################# COMPILE FLAGS ##################################
add_library(release-flags INTERFACE)
target_compile_options(release-flags
    INTERFACE
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
    -O3
    -flto
    -march=native
    -fno-plt
    -fno-semantic-interposition
    -fno-omit-frame-pointer)
target_link_options(release-flags
    INTERFACE
    -flto)
add_library(debug-flags INTERFACE)
target_compile_options(debug-flags
    INTERFACE
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
    -gdwarf-4
    -O3
    -flto
    -march=native
    -fno-plt
    -fno-semantic-interposition
    -fno-omit-frame-pointer)
target_link_options(debug-flags
    INTERFACE
    -flto)

#### core
add_library(supporting
    ${CMAKE_CURRENT_SOURCE_DIR}/benchmark.c
    ${CMAKE_CURRENT_SOURCE_DIR}/loader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log.c)
target_compile_options(supporting
     PRIVATE
     -flto)
target_link_options(supporting
     PRIVATE
     -flto)
target_link_libraries(supporting
    PRIVATE
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)

#### Generated source files for targets
add_custom_command(
    OUTPUT 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_tail-call.c
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/switch.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/tail-call.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_tail-call.c
    DEPENDS 
        ${CMAKE_CURRENT_SOURCE_DIR}/switch.c 
        ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c
        ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c
        ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh
    COMMENT "Expanding all C-Preprocessor macros ..."
    VERBATIM)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_tail-call.c
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c
    PROPERTIES GENERATED TRUE)
############################# TARGET ##################################
add_executable(testsuite
    ${CMAKE_CURRENT_SOURCE_DIR}/testsuite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/switch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c
    ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c)

add_executable(switch        ${CMAKE_CURRENT_SOURCE_DIR}/switch.c)
add_executable(computed-goto ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c)
add_executable(tail-call     ${CMAKE_CURRENT_SOURCE_DIR}/tail-call.c)
add_executable(blocked       ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c)

add_executable(profile_switch        ${CMAKE_CURRENT_SOURCE_DIR}/switch.c)
add_executable(profile_computed-goto ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c)
add_executable(profile_tail-call     ${CMAKE_CURRENT_SOURCE_DIR}/tail-call.c)
add_executable(profile_blocked       ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c)

add_executable(expanded_switch        ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c)
add_executable(expanded_computed-goto ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c)
add_executable(expanded_tail-call     ${CMAKE_CURRENT_BINARY_DIR}/expanded_tail-call.c)
add_executable(expanded_blocked       ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c)

target_compile_definitions(testsuite     PRIVATE __TESTSUITE_MAIN__)
target_compile_definitions(switch        PRIVATE __SWITCH_MAIN__)
target_compile_definitions(computed-goto PRIVATE __COMPUTED_GOTO_MAIN__)
target_compile_definitions(tail-call     PRIVATE __TAIL_CALL_MAIN__)
target_compile_definitions(blocked       PRIVATE __BLOCKED_MAIN__)

target_compile_definitions(profile_switch        PRIVATE __PROFILE__ __SWITCH_MAIN__)
target_compile_definitions(profile_computed-goto PRIVATE __PROFILE__ __COMPUTED_GOTO_MAIN__)
target_compile_definitions(profile_tail-call     PRIVATE __PROFILE__ __TAIL_CALL_MAIN__)
target_compile_definitions(profile_blocked       PRIVATE __PROFILE__ __BLOCKED_MAIN__)

target_compile_definitions(expanded_switch        PRIVATE __SWITCH_MAIN__)
target_compile_definitions(expanded_computed-goto PRIVATE __COMPUTED_GOTO_MAIN__)
target_compile_definitions(expanded_tail-call     PRIVATE __TAIL_CALL_MAIN__)
target_compile_definitions(expanded_blocked       PRIVATE __BLOCKED_MAIN__)

target_include_directories(testsuite     PRIVATE .  tests)
target_include_directories(switch        PRIVATE .)
target_include_directories(computed-goto PRIVATE .)
target_include_directories(tail-call     PRIVATE .)
target_include_directories(blocked       PRIVATE .)

target_include_directories(profile_switch        PRIVATE .)
target_include_directories(profile_computed-goto PRIVATE .)
target_include_directories(profile_tail-call     PRIVATE .)
target_include_directories(profile_blocked       PRIVATE .)

target_include_directories(expanded_switch        PRIVATE .)
target_include_directories(expanded_computed-goto PRIVATE .)
target_include_directories(expanded_tail-call     PRIVATE .)
target_include_directories(expanded_blocked       PRIVATE .)

target_link_libraries(testsuite     PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(switch        PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(computed-goto PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(tail-call     PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(blocked       PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)

target_link_libraries(profile_switch        PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(profile_computed-goto PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(profile_tail-call     PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(profile_blocked       PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)

target_link_libraries(expanded_switch        PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(expanded_computed-goto PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(expanded_tail-call     PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(expanded_blocked       PRIVATE supporting $<$<CONFIG:RELEASE>: release-flags> $<$<CONFIG:DEBUG>: debug-flags>)
