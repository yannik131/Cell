#!/bin/bash

# Echo commands
set -x

# Abort upon failure
set -e

# Change to workspace folder
cd "$(dirname "$0")/../../"

# Check if contrib/vcpkg exists
if [ ! -d "contrib/vcpkg/buildtrees/qtbase" ]; then
    git submodule update --init
    ./contrib/vcpkg/bootstrap-vcpkg.sh
fi

#required on mac/linux:
# autoconf autoconf-archive automake make libtool curl zip unzip pkgconfig

export MACOSX_DEPLOYMENT_TARGET=14.0
export VCPKG_KEEP_ENV_VARS=MACOSX_DEPLOYMENT_TARGET
./contrib/vcpkg/vcpkg install glog sfml nanoflann qtbase

CONFIGURE_OPTIONS=""
if [[ "$OSTYPE" == "darwin"* ]]; then
    CONFIGURE_OPTIONS="-DCMAKE_OSX_ARCHITECTURES=arm64"
fi

# Release build
if [ ! -d "build_release" ]; then
    mkdir build_release
    cd build_release
    cmake .. -DCMAKE_BUILD_TYPE=Release $CONFIGURE_OPTIONS
    cmake --build . --config Release --parallel 4
    cd ..
fi

# Debug build
if [ ! -d "build_debug" ]; then
    mkdir build_debug
    cd build_debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON $CONFIGURE_OPTIONS
    cmake --build . --config Debug --parallel 4
    cd ..
fi