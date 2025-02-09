# echo commands
Set-PSDebug -Trace 1

# abort upon failure
$ErrorActionPreference = "Stop"

# change to workspace folder
Set-Location -Path $PSScriptRoot
Set-Location .\..\..

# create the virtual environment to avoid installing conan globally
if (-Not (Test-Path "env\Scripts\activate")) {
    python -m venv env
    .\env\Scripts\pip install conan
}

# build the project
.\env\Scripts\activate
conan profile detect --force
conan install . -s build_type=Release --output-folder=conan_release --build=missing --profile=profile_windows
conan install . --output-folder=conan_debug --build=missing -s build_type=Debug --profile=profile_windows

# Release build
if (-Not (Test-Path "build_release")) {
    New-Item -ItemType Directory -Path "build_release"
}
Set-Location -Path "build_release"
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel 4
Set-Location ..

# Debug build
if (-Not (Test-Path "build_debug")) {
    New-Item -ItemType Directory -Path "build_debug"
}
Set-Location -Path "build_debug"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build . --config Debug --parallel 4
Set-Location ..