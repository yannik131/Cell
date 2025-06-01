#!/bin/bash

# Echo commands
set -x

# Abort upon failure
set -e

# Change to workspace folder
cd "$(dirname "$0")/../../"

# common vcpkg dependencies include (also see https://learn.microsoft.com/en-us/vcpkg/concepts/supported-hosts)
# apt install autoconf-archive flex bison curl zip unzip tar pkgconfig autoconf automake libtool build-essentials ninja-build
# for qt-base we also need:
# apt install '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev libegl1-mesa-dev

# Check if contrib/vcpkg exists
if [ ! -d "contrib/vcpkg/" ]; then
    git submodule update
    ./contrib/vcpkg/bootstrap-vcpkg.sh
fi

# Release build
if [ ! -d "build_release" ]; then
    mkdir build_release
    cd build_release
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release --parallel 4
    cd ..
fi

# Debug build
if [ ! -d "build_debug" ]; then
    mkdir build_debug
    cd build_debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build . --config Debug --parallel 4
    cd ..
fi