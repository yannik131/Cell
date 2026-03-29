add_library(qcustomplot 
    qcustomplot.h 
    qcustomplot.cpp
)

set_target_properties(qcustomplot PROPERTIES AUTOMOC ON)
target_include_directories(qcustomplot PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(qcustomplot 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::PrintSupport
)