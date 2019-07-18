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

        ${GLEW_INCLUDE_DIR} # not currently used. must fix glfw/glew entanglement
        ${glfw_INCLUDE_DIR} # this was required on windows to get access to glew. This is a good reason to separate the two # TODO: SEPARATE GLFW from GLEW

    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
        
        #${gdkmath_INCLUDE_DIRECTORIES} #This had to be promoted in order to get the include path for THIS project. that is a bug. regardless of public or private, this project should have the path, then only publics are propegated downstream.
        #${gdkglfwwrapper_INCLUDE_DIRECTORIES} # This is required for Windows because glew is hidden in glfw. These must be separated.

        ${stb_INCLUDE_DIR}
        ${nlohmann_INCLUDE_DIR}

    LIBRARIES
        ${gdkmath_LIBRARIES}

        ${GLEW_LIBRARIES} # not currently used. must fix glfw/glew entanglement
        ${glfw_LIBRARIES} # Separate GLEW, you fool
        ${stb_LIBRARIES}

    DEPENDENCIES
        "stb"
        "gdkmath"

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/color.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/floatuniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/glh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mat4x4uniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaderprogram.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/textureuniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector2uniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector3uniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vector4uniformcollection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertexattribute.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertexdata.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertexformat.cpp
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

