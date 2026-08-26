# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

project(null)

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

jfc_project(library
    NAME "gdkgraphics"
    VERSION 0.9
    DESCRIPTION "game development kit graphics lib"
    C++_STANDARD 20
    C_STANDARD 90
    TYPE STATIC

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/include
        
        ${gdkmath_INCLUDE_DIRECTORIES} 
        ${gdktime_INCLUDE_DIRECTORIES}
        ${gdkmemory_INCLUDE_DIRECTORIES}
        ${jfc-smart_handles_INCLUDE_DIRECTORIES}
        ${gdkwindow_INCLUDE_DIRECTORIES}

    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include

        ${CMAKE_CURRENT_SOURCE_DIR}/incubate
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/opengl/webgl1es2/src/include

        ${stb_INCLUDE_DIR}
        ${cgltf_INCLUDE_DIR}

    LIBRARIES
        ${gdkmath_LIBRARIES}
        ${gdktime_LIBRARIES}
        ${stb_LIBRARIES}
        ${cgltf_LIBRARIES}
        ${gdkwindow_LIBRARIES} # Required by provided webgl/gles2 implementation

    DEPENDENCIES
        "stb"
        "cgltf"
        "gdkmath"
        "gdktime"
        "gdkwindow" # Required by provided webgl/gles2 implementation

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/gltf.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/bvh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/ik.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/profile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/retarget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/skeleton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/skinning.cpp

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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/attribute_data.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/color.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/batch_modeler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/sprite_animation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model_data.cpp
)

option(GDK_GRAPHICS_EXT_PNG "Build the ext/ png decoder" ON)

if (GDK_GRAPHICS_EXT_PNG)
    add_library(gdkgraphics_ext_png STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/png.cpp")

    target_compile_features(gdkgraphics_ext_png PUBLIC cxx_std_20)

    target_include_directories(gdkgraphics_ext_png
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
            ${gdkgraphics_INCLUDE_DIRECTORIES}
        PRIVATE ${stb_INCLUDE_DIR})

    target_link_libraries(gdkgraphics_ext_png PUBLIC gdkgraphics ${stb_LIBRARIES})

    set(gdkgraphics_ext_png_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_SOURCE_DIR}/include" ${gdkgraphics_INCLUDE_DIRECTORIES})
    set(gdkgraphics_ext_png_LIBRARIES gdkgraphics_ext_png)
endif()

if (JFC_BUILD_DEMO)
    add_subdirectory(demo)
endif()

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

