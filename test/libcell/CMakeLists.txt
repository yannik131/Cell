file(GLOB_RECURSE LIBCELL_TEST_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(tests-libcell ${LIBCELL_TEST_SRC})

target_link_libraries(tests-libcell
    libcell
    GTest::gtest
)

target_include_directories(tests-libcell PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/src/lib/
)

include(GoogleTest)
gtest_add_tests(TARGET tests-libcell)