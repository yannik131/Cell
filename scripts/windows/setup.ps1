# echo commands
Set-PSDebug -Trace 1

# abort upon failure
$ErrorActionPreference = "Stop"

# change to workspace folder
Set-Location -Path $PSScriptRoot
Set-Location .\..\..

if (-Not (Test-Path "contrib\vcpkg\buildtrees\")) {
    git submodule update --init
    .\contrib\vcpkg\bootstrap-vcpkg.bat
    .\contrib\vcpkg\vcpkg.exe install glog sfml nanoflann qtbase gtest
}

# Release build
if (-Not (Test-Path "build_release")) {
    New-Item -ItemType Directory -Path "build_release"
    Set-Location -Path "build_release"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release --parallel 4
    Set-Location ..
    .\contrib\vcpkg\installed\x64-windows\tools\Qt6\bin\windeployqt6.exe .\build_release\Release\qt-gui.exe
}

# Debug build
if (-Not (Test-Path "build_debug")) {
    New-Item -ItemType Directory -Path "build_debug"
    Set-Location -Path "build_debug"
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build . --config Debug --parallel 4
    Set-Location ..
    .\contrib\vcpkg\installed\x64-windows\tools\Qt6\bin\windeployqt.debug.bat .\build_debug\Debug\qt-gui.exe
}
