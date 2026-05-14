file(GLOB LIBCELL_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(libcell ${LIBCELL_SRC})
target_include_directories(libcell PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(libcell PUBLIC
    glog::glog
    nlohmann_json::nlohmann_json
)