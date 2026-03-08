#!/usr/bin/env bash

# -- download musl-cross-make
if [ ! -d ./musl-cross-make ]; then
    git clone https://github.com/richfelker/musl-cross-make.git
fi

if [ ! -f ./musl-cross-make/config.mak ]; then
    cp ./toolchain_config.mak ./musl-cross-make/config.mak
fi

cd musl-cross-make
make -j4
make install
