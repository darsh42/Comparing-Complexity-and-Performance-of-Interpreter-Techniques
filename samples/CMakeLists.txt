cmake_minimum_required(VERSION 3.16)

include(ExternalProject)

project(mips-binary
    VERSION 1.0
    DESCRIPTION "sample mips binaries"
    LANGUAGES C)

## libraries
add_library(compile-flags INTERFACE)

## compiler options
target_compile_options(compile-flags
    INTERFACE
    -static
    -fno-pie
    -Wall 
    -O2
    -mfp32
    -march=r3000
    -mexplicit-relocs
    -fno-delayed-branch  # Note: This affects branches, not loads
    -Wa,-mips1,-O)       # Pass options to the assembler

target_link_options(compile-flags
    INTERFACE
    -static)

ExternalProject_Add(npb_benchmarks
    URL https://www.nas.nasa.gov/assets/npb/NPB3.4.4.tar.gz
    SOURCE_DIR "${CMAKE_BINARY_DIR}/NPB-src"
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy 
                      ${CMAKE_CURRENT_SOURCE_DIR}/npb_make.def 
                      <SOURCE_DIR>/NPB3.4-OMP/config/make.def
    BUILD_COMMAND make -C <SOURCE_DIR>/NPB3.4-OMP IS CLASS=W
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/bin
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    <SOURCE_DIR>/NPB3.4-OMP/bin/
                    ${CMAKE_CURRENT_BINARY_DIR}/bin)

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
