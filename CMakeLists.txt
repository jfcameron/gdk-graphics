# © 2018 Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_APP   "Build the app" ON)
option(JFC_BUILD_DOCS  "Build documentation" ON)
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

    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
        
        ${gdkmath_INCLUDE_DIRECTORIES} 
        ${gdkmemory_INCLUDE_DIRECTORIES} # Move this to a submodule, hide headers in private header directory.
        #${gdkglfwwrapper_INCLUDE_DIRECTORIES} # This is required for Windows because glew is hidden in glfw. These must be separated.

        ${stb_INCLUDE_DIR}
        ${glfw_INCLUDE_DIR} # this was required on windows to get access to glew. This is a good reason to separate the two # TODO: SEPARATE GLFW from GLEW
        ${nlohmann_INCLUDE_DIR}

    LIBRARIES
        ${gdkmath_LIBRARIES}
        
        ${stb_LIBRARIES}

    DEPENDENCIES
        "stb"
        "gdkmath"

    SOURCE_LIST
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/color.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/floatuniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/glh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mat4x4uniformcollection.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaderprogram.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/textureuniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector2uniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector3uniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector4uniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertexattribute.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertexdata.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertexformat.cpp
)

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

