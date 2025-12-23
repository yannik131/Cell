#!/bin/bash

cd "$(dirname "$0")"
cd ../..

if [[ "$1" == "-f" ]]; then
    echo "Formatting files..."
    find src/ include/ test/ -name '*.h' -o -name '*.hpp' -o -name '*.inl' -o -name '*.cpp' | xargs clang-format -i
else
    echo "Checking with clang-format..."
    find src/ include/ test/ -name '*.h' -o -name '*.hpp' -o -name '*.inl' -o -name '*.cpp' | xargs clang-format --dry-run --Werror
fi