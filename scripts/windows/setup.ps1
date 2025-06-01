# to build in 64 bit mode on windows with ninja and use clang-tidy:
# - open x64 native tools command prompt for vs 2022
# - obtain these env vars: echo %PATH%, echo %LIB%, echo %INCLUDE%
# - in the shell for building, set PATH, LIB and INCLUDE to those values and run:
# cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl ..
# - in vs code, add to .vscode/settings.json:
# { ... "cmake.environment": { "PATH": ..., "LIB": ..., "INCLUDE": ... } }

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
}

# Release build
if (-Not (Test-Path "build_release")) {
    New-Item -ItemType Directory -Path "build_release"
    Set-Location -Path "build_release"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release --parallel 4
    Set-Location ..
    .\build_release\vcpkg_installed\x64-windows\tools\Qt6\bin\windeployqt.exe .\build_release\cell-gui.exe
}

# Debug build
if (-Not (Test-Path "build_debug")) {
    New-Item -ItemType Directory -Path "build_debug"
    Set-Location -Path "build_debug"
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build . --config Debug --parallel 4
    Set-Location ..
    .\build_debug\vcpkg_installed\x64-windows\tools\Qt6\bin\windeployqt.debug.bat .\build_debug\cell-gui.exe
}
