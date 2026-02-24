cmake_minimum_required(VERSION 3.16)

project(mips-binary
    VERSION 1.0
    DESCRIPTION "bare metal mips binary"
    LANGUAGES C)

## set the correct mips c compiler
set(CMAKE_C_COMPILER mipsel-linux-gnu-gcc)

## libraries
add_library(compile-flags INTERFACE)

## compiler options
target_compile_options(compile-flags
    INTERFACE
    -static
    -Wall 
    -O2
    -march=mips1 
    -mfp32)

## executables
add_executable(binary_search         ${CMAKE_CURRENT_SOURCE_DIR}/binary_search.c)
add_executable(convolution           ${CMAKE_CURRENT_SOURCE_DIR}/convolution.c)
add_executable(crc                   ${CMAKE_CURRENT_SOURCE_DIR}/crc.c)
add_executable(daxpy                 ${CMAKE_CURRENT_SOURCE_DIR}/daxpy.c)
add_executable(dot_product           ${CMAKE_CURRENT_SOURCE_DIR}/dot_product.c)
add_executable(fft                   ${CMAKE_CURRENT_SOURCE_DIR}/fft.c)
add_executable(horner_polynomial     ${CMAKE_CURRENT_SOURCE_DIR}/horner_polynomial.c)
add_executable(linked_list           ${CMAKE_CURRENT_SOURCE_DIR}/linked_list.c)
add_executable(lu_decomposition      ${CMAKE_CURRENT_SOURCE_DIR}/lu_decomposition.c)
add_executable(matrix_multiplication ${CMAKE_CURRENT_SOURCE_DIR}/matrix_multiplication.c)
add_executable(quicksort             ${CMAKE_CURRENT_SOURCE_DIR}/quicksort.c)

target_link_libraries(binary_search              PRIVATE compile-flags)
target_link_libraries(convolution                PRIVATE compile-flags)
target_link_libraries(crc                        PRIVATE compile-flags)
target_link_libraries(daxpy                      PRIVATE compile-flags)
target_link_libraries(dot_product                PRIVATE compile-flags)
target_link_libraries(fft                        PRIVATE compile-flags)
target_link_libraries(horner_polynomial          PRIVATE compile-flags)
target_link_libraries(linked_list                PRIVATE compile-flags)
target_link_libraries(lu_decomposition           PRIVATE compile-flags)
target_link_libraries(matrix_multiplication      PRIVATE compile-flags)
target_link_libraries(quicksort                  PRIVATE compile-flags)
