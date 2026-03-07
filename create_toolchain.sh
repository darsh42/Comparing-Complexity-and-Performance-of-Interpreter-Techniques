#!/usr/bin/env bash

# -- create build tools directory
mkdir -p build-tools

# -- download mipsel-linux-musl-cross compiler
if [ ! -f ./build-tools/mipsel-linux-musl-cross.tgz ]; then
    wget -P ./build-tools https://musl.cc/mipsel-linux-musl-cross.tgz
fi

# -- untar the compiler
if [ ! -d ./build-tools/bin ]; then
    tar -xzf ./build-tools/mipsel-linux-musl-cross.tgz \
        -C ./build-tools                               \
        --strip-components=1
fi

# -- add build-tools directory to PATH
export PATH="./build-tools/bin:$PATH"
