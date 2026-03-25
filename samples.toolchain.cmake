set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR mips)

# Adjust this path to where musl-cross-make actually installs
set(TOOLCHAIN_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/../tools")

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}/bin/mipsel-linux-muslsf-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}/bin/mipsel-linux-muslsf-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
