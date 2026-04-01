#!/bin/bash

cd "$(dirname "$0")"
cd ../..

output=$(run-clang-tidy -p build_debug -j 4 \
    -source-filter="^.*/Cell/(src|test)/.*" \
    -header-filter="^.*/Cell/(src|test)/.*" \
    2>&1 \
    | grep -v "src/apps/playground/main.cpp" \
    | tee /dev/stderr)

if echo "$output" | grep -E "warning:|error:" > /dev/null; then
    echo "clang-tidy found warnings/errors. You can rerun it with --fix or fix them yourself."
    exit 1
fi