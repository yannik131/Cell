#! /bin/bash

cd "$(dirname "$0")"
cd ../..

output=$(run-clang-tidy -p build_release -j 4 -source-filter=".*src/.*|.*test/.*" -header-filter=".*src/.*|.*test/.*" | tee /dev/stderr)

if echo $output | grep -E "warning:|error:" > /dev/null; then 
    echo "clang-tidy found warnings/errors. You can rerun it with --fix or fix them yourself."
    exit 1
fi 

