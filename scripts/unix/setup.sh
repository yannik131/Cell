#!/bin/bash

# echo commands
set -x

# abort upon failure
set -e

# change to workspace folder
cd "$(dirname "$0")"
cd ../..

# create the virtual environment to avoid installing conan globally
if [ ! -d "env/bin" ] || [ ! -f "env/bin/activate" ]; then
    python3 -m venv env
    pip3 install conan
fi

# build the project
source env/bin/activate
conan profile detect --force
conan install . -s build_type=Release --output-folder=conan_release --build=missing
conan install . -s build_type=Debug --output-folder=conan_debug --build=missing -s
mkdir -p build_release
cd build_release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel 4
cd ..
mkdir -p build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build . --config Debug --parallel 4
