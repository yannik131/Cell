#!/bin/bash

cd "$(dirname "$0")"
cd ..

if [[ "$1" == "-c" ]]; then
    echo "Checking with clang-format..."
    find src/ include/ test/ -name '*.h' -o -name '*.hpp' -o -name '*.inl' -o -name '*.cpp' | xargs clang-format --dry-run --Werror

    echo "Checking with clang-tidy..."
    find src/ include/ -name '*.h' -o -name '*.hpp' -o -name '*.inl' -o -name '*.cpp' | xargs clang-tidy -p=build_debug
else
    echo "Formatting files..."
    find src/ include/ test/ -name '*.h' -o -name '*.hpp' -o -name '*.inl' -o -name '*.cpp' | xargs clang-format -i
    
    echo "Letting clang-tidy try to fix stuff..."
    find src/ include/ -name '*.h' -o -name '*.hpp' -o -name '*.inl' -o -name '*.cpp' | xargs clang-tidy -p=build_debug --fix
fi