add_executable(cell-gui WIN32 Main.cpp)

if(WIN32)
    target_sources(cell-gui PRIVATE cell-gui.rc)
endif()

target_link_libraries(cell-gui PRIVATE libcellgui)