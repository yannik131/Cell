#!/bin/bash

cd "$(dirname "$0")"
cd ../..

if [[ "$1" == "-f" ]]; then
    echo "Letting clang-tidy try to fix stuff..."
    find src/ include/ -name '*.h' -o -name '*.hpp' -o -name '*.inl' -o -name '*.cpp' | xargs clang-tidy -p=build_debug --fix
else
    echo "Checking with clang-tidy..."
    find src/ include/ -name '*.h' -o -name '*.hpp' -o -name '*.inl' -o -name '*.cpp' | xargs clang-tidy -p=build_debug
fi