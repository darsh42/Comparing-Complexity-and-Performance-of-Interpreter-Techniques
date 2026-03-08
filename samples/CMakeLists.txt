cmake_minimum_required(VERSION 3.16)

project(mips-binary
    VERSION 1.0
    DESCRIPTION "bare metal mips binary"
    LANGUAGES C)

## set the correct mips c compiler
set(CMAKE_C_COMPILER ../build-tools/bin/mipsel-linux-muslsf-gcc)

## libraries
add_library(compile-flags INTERFACE)

## compiler options
target_compile_options(compile-flags
    INTERFACE
    -static
    -fno-pie
    -Wall 
    -g
    -mfp32
    -march=r3000
    -mexplicit-relocs
    -fno-delayed-branch  # Note: This affects branches, not loads
    -Wa,-mips1,-O)       # Pass options to the assembler

target_link_options(compile-flags
    INTERFACE
    -static)

## executables
add_executable(binary_search ${CMAKE_CURRENT_SOURCE_DIR}/binary_search.c)
add_executable(crc           ${CMAKE_CURRENT_SOURCE_DIR}/crc.c)
add_executable(daxpy         ${CMAKE_CURRENT_SOURCE_DIR}/daxpy.c)
add_executable(linked_list   ${CMAKE_CURRENT_SOURCE_DIR}/linked_list.c)
add_executable(quicksort     ${CMAKE_CURRENT_SOURCE_DIR}/quicksort.c)

target_link_libraries(binary_search PRIVATE compile-flags)
target_link_libraries(crc           PRIVATE compile-flags)
target_link_libraries(daxpy         PRIVATE compile-flags)
target_link_libraries(linked_list   PRIVATE compile-flags)
target_link_libraries(quicksort     PRIVATE compile-flags)
