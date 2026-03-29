add_executable(benchmark Main.cpp)
target_link_libraries(benchmark libcell)
target_include_directories(benchmark PUBLIC ${CMAKE_SOURCE_DIR}/src/lib/)