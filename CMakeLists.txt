cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW) # conan does not set this but cmake wants it
cmake_policy(SET CMP0057 NEW) # again..
project(Cell)

message("Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(DEBUG)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/contrib/vcpkg/scripts/buildsystems/vcpkg.cmake)

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

find_package(SFML COMPONENTS system window graphics REQUIRED)
find_package(glog REQUIRED)
find_package(nanoflann REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets PrintSupport OpenGLWidgets REQUIRED)

add_compile_definitions(GLOG_USE_GLOG_EXPORT)

file(GLOB LIBCELL_SRC src/libcell/*.cpp)
add_library(libcell ${LIBCELL_SRC})

target_include_directories(libcell PUBLIC ${CMAKE_SOURCE_DIR}/include/libcell)

target_link_libraries(libcell PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system
    nanoflann::nanoflann
)

# qcustomplot
add_library(qcustomplot ${CMAKE_SOURCE_DIR}/contrib/qcustomplot/qcustomplot.h ${CMAKE_SOURCE_DIR}/contrib/qcustomplot/qcustomplot.cpp)
set_target_properties(qcustomplot PROPERTIES AUTOMOC ON)
target_include_directories(qcustomplot PUBLIC ${CMAKE_SOURCE_DIR}/contrib/qcustomplot/)
target_link_libraries(qcustomplot Qt6::Core Qt6::Widgets Qt6::PrintSupport)

# the gui target
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB QT_GUI_SRC src/qt-gui/*.cpp src/gui/*.ui include/qt-gui/*.hpp)

#the header files have to be added here as well for Q_OBJECT to work
add_executable(qt-gui
  ${QT_GUI_SRC} 
)

target_include_directories(qt-gui PUBLIC ${CMAKE_SOURCE_DIR}/include/qt-gui/)

target_link_libraries(qt-gui libcell glog::glog Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets qcustomplot)