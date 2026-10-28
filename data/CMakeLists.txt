# © Joseph Cameron - MIT License

add_library(gdkgraphics_data STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/attribute_data.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/color.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/model_data.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/bvh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/ik.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/profile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/retarget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/skeleton.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/animation/skinning.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/aseprite.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/cgltf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/font.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/font_check.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/gltf.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/png.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/sprite_animation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/stb.c")

add_library(gdk::graphics_data ALIAS gdkgraphics_data)

set_target_properties(gdkgraphics_data PROPERTIES EXPORT_NAME graphics_data PREFIX "lib" C_STANDARD 99)

target_include_directories(gdkgraphics_data
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${PROJECT_SOURCE_DIR}/extern")

target_compile_features(gdkgraphics_data PUBLIC cxx_std_20)

target_link_libraries(gdkgraphics_data PUBLIC gdkmath)

set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/cgltf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/stb.c"
    PROPERTIES COMPILE_OPTIONS "$<$<C_COMPILER_ID:MSVC>:/WX->")

if (GDKGRAPHICS_BUILD_TESTS)
    add_subdirectory(fuzz)
endif()
