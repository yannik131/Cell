add_executable(playground Main.cpp)

target_include_directories(playground PRIVATE ${CMAKE_SOURCE_DIR}/contrib/qcustomplot/)

target_link_libraries(playground PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    nlohmann_json::nlohmann_json
    Boost::histogram
    qcustomplot
)

target_precompile_headers(playground PRIVATE
    "${CMAKE_SOURCE_DIR}/contrib/qcustomplot/qcustomplot.h"
)