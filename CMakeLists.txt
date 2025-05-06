# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/jfc-cmake/jfclib.cmake")

project(null)

option(JFC_BUILD_DEMO "Build the demo" ON)
option(JFC_BUILD_DOCS "Build documentation" OFF)
option(JFC_BUILD_TESTS "Build unit tests" OFF)

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
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/include
        
        ${gdkmath_INCLUDE_DIRECTORIES} 
        ${gdkmemory_INCLUDE_DIRECTORIES}
        ${jfc-smart_handles_INCLUDE_DIRECTORIES}
        ${simpleglfw_INCLUDE_DIRECTORIES}

    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/include

        ${stb_INCLUDE_DIR}

    LIBRARIES
        ${gdkmath_LIBRARIES}
        ${stb_LIBRARIES}
        
        ${simpleglfw_LIBRARIES} # TODO: wrong. Split this up. gfx depend on OpenGL headers, not on glfw 
        # ^ need to move the demo into a separate repo. gdk-graphics knows nothing about windowing systems

    DEPENDENCIES
        "stb"
        "gdkmath"
        "simpleglfw"

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/color.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vertex_data.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/texture_data.cpp
                
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/common/src/glh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_camera.cpp  
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_entity.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_material.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_screen_camera.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_shader_program.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/webgl1es2_texture_camera.cpp
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

