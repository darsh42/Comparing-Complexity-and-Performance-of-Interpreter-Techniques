#!/usr/bin/env bash
gcc -D__MACRO_EXPANSION__ -E -P -C -I. -Itemplates -Itests $1 | clang-format
