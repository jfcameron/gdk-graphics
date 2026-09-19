# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

project(gdkgraphics
    VERSION 0.9
    DESCRIPTION "game development kit graphics lib")

option(JFC_BUILD_DEMO "Build the demo" ON)
option(JFC_BUILD_DOCS "Build documentation" OFF)
option(JFC_BUILD_TESTS "Build unit tests" OFF)

add_subdirectory(extern)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

add_library(gdkgraphics STATIC
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/batch_modeler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/text_modeler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/null/src/null_context.cpp)

add_library(gdk::graphics ALIAS gdkgraphics)

set_target_properties(gdkgraphics PROPERTIES EXPORT_NAME graphics)

set_target_properties(gdkgraphics PROPERTIES PREFIX "lib")

jfc_generate_build_info()

target_include_directories(gdkgraphics
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include>"
        "$<INSTALL_INTERFACE:include>"

        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated_include>"
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
        ${CMAKE_CURRENT_SOURCE_DIR}/incubate
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/include
    )

target_compile_features(gdkgraphics PUBLIC cxx_std_20)

target_link_libraries(gdkgraphics PUBLIC
    gdkgraphics_data
    gdkmath
    gdktime
    gdkmemory
    jfc-smart_handles
    gdkwindow
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

