# © Joseph Cameron - All Rights Reserved

jfc_project(executable
    NAME "simple_text_demo"
    VERSION 1.0
    DESCRIPTION "demo gdk-graphics app"
    C++_STANDARD 20
    C_STANDARD 90

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/text_modeler.cpp
    
    PRIVATE_INCLUDE_DIRECTORIES
        "${gdkgraphics_INCLUDE_DIRECTORIES}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../incubate"

    LIBRARIES
        "${gdkgraphics_LIBRARIES}"
        "${gdkgraphics_ext_png_LIBRARIES}"

    DEPENDENCIES
        "gdkgraphics"
)

