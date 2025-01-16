cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0091 NEW) # conan does not set this but cmake wants it
cmake_policy(SET CMP0057 NEW) # again..
project(Cell)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(${CMAKE_SOURCE_DIR}/conan_debug/conan_toolchain.cmake)
  add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(${CMAKE_SOURCE_DIR}/conan_release/conan_toolchain.cmake)
else()
    message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# c++17 for std::filesystem
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# the simulation-base target
file(GLOB LIB_SRC_FILES src/simulation-base/*.cpp)
add_library(simulation-base STATIC ${LIB_SRC_FILES})

target_include_directories(simulation-base PUBLIC 
  ${CMAKE_SOURCE_DIR}/include/simulation-base/
)

# the test target
enable_testing()

find_package(GTest REQUIRED)

file(GLOB TEST_SRC_FILES test/simulation-base/*.cpp)
add_executable(simulation-test ${TEST_SRC_FILES})

target_link_libraries(simulation-test
  simulation-base
  GTest::gtest_main
)

include(GoogleTest)
gtest_add_tests(TARGET simulation-test)
