cmake_minimum_required(VERSION 3.20)
project(Cell LANGUAGES CXX)

message("Build type: ${CMAKE_BUILD_TYPE}")

cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0057 NEW)

option(ENABLE_GUI "Build GUI targets" ON)
option(ENABLE_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

if(MSVC)
    add_compile_options(/W4 /wd4244)
else()
    add_compile_options(-Wall)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(DEBUG)
endif()

if(ENABLE_GUI)
    # On Linux, Qt6 must be found before sfml, otherwise sfml will re-define a target (egl) and screw things up
    find_package(Qt6 COMPONENTS Core Widgets PrintSupport OpenGLWidgets Test REQUIRED)
    find_package(SFML COMPONENTS System Window Graphics REQUIRED)
    add_subdirectory(contrib/qcustomplot)
    add_subdirectory(src/lib/gui)
    add_subdirectory(src/apps/cell-gui)
endif()

if(ENABLE_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()
    add_subdirectory(test/libcell)
endif()

find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost COMPONENTS histogram REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

add_subdirectory(src/lib/cell)
add_subdirectory(src/apps/benchmark)
add_subdirectory(src/apps/playground)
add_subdirectory(src/apps/cell-cli)