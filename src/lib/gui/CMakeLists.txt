file(GLOB_RECURSE QT_GUI_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
)

add_library(libcellgui
  ${QT_GUI_SRC} 
)

target_include_directories(libcellgui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/lib/
    ${CMAKE_SOURCE_DIR}/contrib/qcustomplot/
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

set_target_properties(libcellgui PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

target_precompile_headers(libcellgui PRIVATE
    "${CMAKE_SOURCE_DIR}/contrib/qcustomplot/qcustomplot.h"
    <boost/histogram.hpp>
)