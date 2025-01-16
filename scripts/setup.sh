#!/bin/bash

# echo commands
set -x

# change to workspace folder
cd "$(dirname "$0")"
cd ..

# create the virtual environment to avoid installing conan globally
if [ ! -d "env/bin" ] || [ ! -f "env/bin/activate" ]; then
    python3 -m venv env
    pip3 install conan
fi

# build the project
source env/bin/activate
conan install . --output-folder=conan_release --build=missing
conan install . --output-folder=conan_debug --build=missing -s build_type=Debug
mkdir build_release
cd build_release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ..
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build . --config Debug 
