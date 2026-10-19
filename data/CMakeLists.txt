# © Joseph Cameron - All Rights Reserved

project(gdkgraphics_data
    VERSION 0.9
    DESCRIPTION "game development kit graphics data lib")

add_library(gdkgraphics_data STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/attribute_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/bvh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/ik.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/profile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/retarget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/skeleton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/skinning.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/aseprite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/font.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/gltf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/png.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ext/sprite_animation.cpp)

add_library(gdk::graphics_data ALIAS gdkgraphics_data)

set_target_properties(gdkgraphics_data PROPERTIES EXPORT_NAME graphics_data)

set_target_properties(gdkgraphics_data PROPERTIES PREFIX "lib")

target_include_directories(gdkgraphics_data
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>")

target_compile_features(gdkgraphics_data PUBLIC cxx_std_20)

target_link_libraries(gdkgraphics_data
    PUBLIC
        gdkmath
    PRIVATE
        stb
        cgltf)

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_FUZZERS)
    add_subdirectory(fuzz)
endif()

