# © Joseph Cameron - All Rights Reserved

jfc_project(executable
    NAME "skeleton_demo"
    VERSION 1.0
    DESCRIPTION "cpu skeletal animation demo"
    C++_STANDARD 20
    C_STANDARD 90

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/content.cpp

    PRIVATE_INCLUDE_DIRECTORIES
        "${gdkgraphics_INCLUDE_DIRECTORIES}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../incubate"

    LIBRARIES
        "${gdkgraphics_LIBRARIES}"

    DEPENDENCIES
        "gdkgraphics"
)
