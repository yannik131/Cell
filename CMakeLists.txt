cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW) # conan does not set this but cmake wants it
cmake_policy(SET CMP0057 NEW) # again..
project(Cell)

message("Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(DEBUG)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(MSVC)
    add_compile_options(/W4 /wd4244)
else()
    add_compile_options(-Wall)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# On Linux, Qt6 must be found before sfml, otherwise sfml will re-define a target (egl) and screw things up
find_package(Qt6 COMPONENTS Core Widgets PrintSupport OpenGLWidgets Test REQUIRED)
find_package(SFML COMPONENTS System Window Graphics REQUIRED)
find_package(glog REQUIRED)
find_package(nanoflann REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_compile_definitions(GLOG_USE_GLOG_EXPORT)

file(GLOB LIBCELL_SRC src/lib/cell/*.cpp)
add_library(libcell ${LIBCELL_SRC})

target_include_directories(libcell PRIVATE ${CMAKE_SOURCE_DIR}/src/lib/cell/)

target_link_libraries(libcell PUBLIC
    nanoflann::nanoflann
    glog::glog
    nlohmann_json::nlohmann_json
)

# qcustomplot
add_library(qcustomplot ${CMAKE_SOURCE_DIR}/contrib/qcustomplot/qcustomplot.h ${CMAKE_SOURCE_DIR}/contrib/qcustomplot/qcustomplot.cpp)
set_target_properties(qcustomplot PROPERTIES AUTOMOC ON)
target_include_directories(qcustomplot PUBLIC ${CMAKE_SOURCE_DIR}/contrib/qcustomplot/)
target_link_libraries(qcustomplot Qt6::Core Qt6::Widgets Qt6::PrintSupport)

# the gui target
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE QT_GUI_SRC ${CMAKE_SOURCE_DIR}
    src/lib/gui/*.cpp 
    src/lib/gui/*.hpp
)

#the header files have to be added here as well for Q_OBJECT to work
add_library(libcellgui
  ${QT_GUI_SRC} 
)

target_include_directories(libcellgui PUBLIC 
    ${CMAKE_SOURCE_DIR}/src/lib/gui/
    ${CMAKE_SOURCE_DIR}/src/lib/
)

target_link_libraries(libcellgui PUBLIC 
    libcell 
    SFML::System
    SFML::Window
    SFML::Graphics
    Qt6::Core 
    Qt6::Widgets 
    Qt6::OpenGLWidgets 
    qcustomplot
)

add_executable(cell-gui WIN32 src/apps/cell-gui/Main.cpp)

if(WIN32)
    target_sources(cell-gui PRIVATE src/apps/cell-gui/cell-gui.rc)
endif()

target_link_libraries(cell-gui libcellgui)

# benchmark target
add_executable(benchmark src/apps/benchmark/Main.cpp)
target_link_libraries(benchmark libcell)
target_include_directories(benchmark PUBLIC ${CMAKE_SOURCE_DIR}/src/lib/)

# target for trying stuff out
add_executable(playground src/apps/playground/Main.cpp)
target_link_libraries(playground PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    nanoflann::nanoflann
    glog::glog
    nlohmann_json::nlohmann_json
)

# tests for cell
enable_testing()

find_package(GTest REQUIRED)

file(GLOB_RECURSE LIBCELL_TEST_SRC ${PROJECT_SOURCE_DIR}/test/libcell/*.cpp)

add_executable(tests-libcell ${LIBCELL_TEST_SRC})

target_link_libraries(tests-libcell
    libcell
    GTest::gtest
)

target_include_directories(tests-libcell PUBLIC 
    ${PROJECT_SOURCE_DIR}/test/libcell/common
    ${PROJECT_SOURCE_DIR}/src/lib/
)

include(GoogleTest)
gtest_add_tests(TARGET tests-libcell)