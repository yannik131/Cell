add_executable(cell-cli Main.cpp)

target_link_libraries(cell-cli PRIVATE libcell gflags)

target_include_directories(cell-cli PRIVATE
    ${CMAKE_SOURCE_DIR}/src/lib/
)