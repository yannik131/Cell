add_executable(cell-cli Main.cpp)

target_link_libraries(cell-cli PRIVATE libcell CLI11::CLI11)

target_include_directories(cell-cli PRIVATE
    ${CMAKE_SOURCE_DIR}/src/lib/
)

target_precompile_headers(cell-cli PRIVATE 
    <CLI/CLI.hpp>
)