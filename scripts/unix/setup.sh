#!/bin/bash

# Echo commands
set -x

# Abort upon failure
set -e

# Change to workspace folder
cd "$(dirname "$0")/../../"

# Check if contrib/vcpkg exists
if [ ! -d "contrib/vcpkg/" ]; then
    git submodule update
    ./contrib/vcpkg/bootstrap-vcpkg.sh
    ./contrib/vcpkg/vcpkg install glog sfml nanoflann qtbase
fi

# Release build
if [ ! -d "build_release" ]; then
    mkdir build_release
    cd build_release
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release --parallel 4
    cd ..
    ./contrib/vcpkg/installed/x64-windows/tools/Qt6/bin/windeployqt6.exe ./build_release/Release/qt-gui.exe
fi

# Debug build
if [ ! -d "build_debug" ]; then
    mkdir build_debug
    cd build_debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build . --config Debug --parallel 4
    cd ..
    ./contrib/vcpkg/installed/x64-windows/tools/Qt6/bin/windeployqt.debug.bat ./build_debug/Debug/qt-gui.exe
fi