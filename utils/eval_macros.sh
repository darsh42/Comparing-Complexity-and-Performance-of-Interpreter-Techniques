#!/usr/bin/env bash
g++ -E -P -C -I. -Itemplates -Itests $1 | clang-format
