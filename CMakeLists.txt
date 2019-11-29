# © 2019 Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_DEMO "Build the demo" ON)
option(JFC_BUILD_DOCS "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" ON)

add_subdirectory(thirdparty)

jfc_project(library
    NAME "gdkgraphics"
    VERSION 0.0
    DESCRIPTION "game development kit graphics lib"
    C++_STANDARD 17
    C_STANDARD 90
    TYPE STATIC

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        
        ${gdkmemory_INCLUDE_DIRECTORIES}
        ${gdkmath_INCLUDE_DIRECTORIES} 
        ${simpleglfw_INCLUDE_DIRECTORIES}
        ${jfc-smart_handles_INCLUDE_DIRECTORIES}

        #${GLEW_INCLUDE_DIR} # not currently used. must fix glfw/glew entanglement
        #${glfw_INCLUDE_DIR} # this was required on windows to get access to glew. This is a good reason to separate the two # TODO: SEPARATE GLFW from GLEW

        #/Users/josephcameron/Workspace/emscripten/system/include/


    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include

        ${stb_INCLUDE_DIR}

    LIBRARIES
        ${gdkmath_LIBRARIES}
        ${simpleglfw_LIBRARIES}
        
        #${GLEW_LIBRARIES} # not currently used. must fix glfw/glew entanglement
        #${glfw_LIBRARIES} # Separate GLEW, you fool
        ${stb_LIBRARIES}

    DEPENDENCIES
        "stb"
        "gdkmath"
        #"simpleglfw" #remove

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/color.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/entity.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/glh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/material.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shader_program.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_attribute.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_format.cpp
)

if (JFC_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

