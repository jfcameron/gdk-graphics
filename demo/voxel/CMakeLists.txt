# © Joseph Cameron - All Rights Reserved

jfc_project(executable
    NAME "voxel_demo"
    VERSION 1.0
    DESCRIPTION "demo gdk-graphics app"
    C++_STANDARD 17
    C_STANDARD 90

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    
    PRIVATE_INCLUDE_DIRECTORIES
        "${gdkgraphics_INCLUDE_DIRECTORIES}"

    LIBRARIES
        "${gdkgraphics_LIBRARIES}"

    DEPENDENCIES
        "gdkgraphics"
)

