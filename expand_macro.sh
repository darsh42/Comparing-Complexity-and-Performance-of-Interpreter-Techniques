#!/usr/bin/env bash

# Check if a file was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <source_file.c>"
    exit 1
fi

ORIGINAL=()
INCLUDES=("#include \"common.h\"")
EXPANDED=$(gcc -D__MACRO_EXPANSION__ -E \
    -P -C -I. -Itemplates -Itests "$1" | clang-format)

in_block=false
while IFS= read -r line; do
    if [[ "$line" == "#ifndef __MACRO_EXPANSION__" ]]; then
        in_block=true; continue 
    fi
    if [[ "$line" == "#endif // __MACRO_EXPANSION__" ]]; then
        in_block=false; continue
    fi

    # Capture content if we are inside a block
    if [ "$in_block" = true ]; then
        ORIGINAL+=("$line")
    fi
done < "$1"

# 3. Output the final assembly
# Print Includes
for inc in "${INCLUDES[@]}"; do
    echo "$inc"
done

echo "$EXPANDED"

for line in "${ORIGINAL[@]}"; do
    echo "$line"
done
