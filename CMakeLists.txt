cmake_minimum_required(VERSION 3.16)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build Type" FORCE)
endif()

project(dissertation
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

#### Generated source files for targets
add_custom_command(
    OUTPUT 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_decoupled.c
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked_chaining.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/switch.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/decoupled.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_decoupled.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh 
        ${CMAKE_CURRENT_SOURCE_DIR}/blocked_chaining.c > 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked_chaining.c
    DEPENDS 
        ${CMAKE_CURRENT_SOURCE_DIR}/switch.c 
        ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c
        ${CMAKE_CURRENT_SOURCE_DIR}/decoupled.c
        ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c
        ${CMAKE_CURRENT_SOURCE_DIR}/blocked_chaining.c
        ${CMAKE_CURRENT_SOURCE_DIR}/expand_macro.sh
    COMMENT "Expanding all C-Preprocessor macros ..."
    VERBATIM)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_decoupled.c
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked_chaining.c
    PROPERTIES GENERATED TRUE)
############################# TARGET ##################################
add_executable(switch
    ${CMAKE_CURRENT_SOURCE_DIR}/switch.c)
add_executable(computed-goto
    ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c)
add_executable(decoupled
    ${CMAKE_CURRENT_SOURCE_DIR}/decoupled.c)
add_executable(blocked
    ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c)
add_executable(blocked_chaining
    ${CMAKE_CURRENT_SOURCE_DIR}/blocked_chaining.c)
add_executable(testsuite
    ${CMAKE_CURRENT_SOURCE_DIR}/testsuite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/switch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/computed-goto.c
    ${CMAKE_CURRENT_SOURCE_DIR}/blocked.c
    ${CMAKE_CURRENT_SOURCE_DIR}/blocked_chaining.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decoupled.c)
add_executable(expanded_switch
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c)
add_executable(expanded_computed-goto
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c)
add_executable(expanded_decoupled
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_decoupled.c)
add_executable(expanded_blocked
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c)
add_executable(expanded_blocked_chaining
    ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked_chaining.c)

target_compile_definitions(switch
    PRIVATE
    __SWITCH_MAIN__)
target_compile_definitions(computed-goto
    PRIVATE
    __COMPUTED_GOTO_MAIN__)
target_compile_definitions(decoupled
    PRIVATE
    __DECOUPLED_MAIN__)
target_compile_definitions(blocked
    PRIVATE
    __BLOCKED_MAIN__)
target_compile_definitions(blocked_chaining
    PRIVATE
    __BLOCKED_CHAINING_MAIN__)
target_compile_definitions(testsuite
    PRIVATE
    __TESTSUITE_MAIN__)
target_compile_definitions(expanded_switch
    PRIVATE
    __SWITCH_MAIN__)
target_compile_definitions(expanded_computed-goto
    PRIVATE
    __COMPUTED_GOTO_MAIN__)
target_compile_definitions(expanded_decoupled
    PRIVATE
    __DECOUPLED_MAIN__)
target_compile_definitions(expanded_blocked
    PRIVATE
    __BLOCKED_MAIN__)
target_compile_definitions(expanded_blocked_chaining
    PRIVATE
    __BLOCKED_CHAINING_MAIN__)

target_include_directories(switch
    PRIVATE .)
target_include_directories(computed-goto
    PRIVATE .)
target_include_directories(decoupled
    PRIVATE .)
target_include_directories(blocked
    PRIVATE .)
target_include_directories(blocked_chaining
    PRIVATE .)
target_include_directories(testsuite
    PRIVATE
    .
    tests)
target_include_directories(expanded_switch
    PRIVATE .)
target_include_directories(expanded_computed-goto
    PRIVATE .)
target_include_directories(expanded_decoupled
    PRIVATE .)
target_include_directories(expanded_blocked
    PRIVATE .)
target_include_directories(expanded_blocked_chaining
    PRIVATE .)

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
target_link_libraries(blocked
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(blocked_chaining
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(testsuite
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(expanded_switch
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(expanded_computed-goto
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(expanded_decoupled
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(expanded_blocked
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)
target_link_libraries(expanded_blocked_chaining
    PRIVATE
    supporting
    $<$<CONFIG:RELEASE>: release-flags>
    $<$<CONFIG:DEBUG>: debug-flags>)

# custom commands for data figures
add_custom_target(analyse-lizard-complexity
    COMMAND lizard 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_switch.c 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_computed-goto.c 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_decoupled.c
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked.c 
        ${CMAKE_CURRENT_BINARY_DIR}/expanded_blocked_chaining.c 
    VERBATIM)
add_custom_target(generate-cfgs
    COMMAND joern-parse ${CMAKE_CURRENT_SOURCE_DIR} --output ${CMAKE_CURRENT_BINARY_DIR}/cpg.bin
    COMMAND joern 
            ${CMAKE_CURRENT_BINARY_DIR}/cpg.bin
            --script ${CMAKE_CURRENT_SOURCE_DIR}/create_cfgs.sc 
            --param output=${CMAKE_CURRENT_BINARY_DIR}/cfgs 
            --param functions="interpreter_switch,interpreter_computed_goto,interpreter_decoupled,interpreter_blocked,interpreter_blocked_chaining"
    
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
    COMMENT "Parsing source and generating Control Flow Graphs"
)
add_custom_target(render-cfgs
    COMMAND find ${CMAKE_CURRENT_BINARY_DIR}/graphs -name "*.dot" -exec ${DOT} -Tpng {} -o {}.png \; 
    COMMENT "Generating Control Flow Graph images ..."
    VERBATIM)
