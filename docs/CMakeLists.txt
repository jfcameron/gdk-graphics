# © 2018 Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

jfc_generate_documentation_doxygen(
    PROJECT_LOGO ${CMAKE_CURRENT_LIST_DIR}/icon.png

    README_PATH  ../README.md
    
    INCLUDE_DIRECTORIES
        ../include/
        ../impl/opengl/webgl1es2/include
)

#jfc_generate_readme_md(
#    BRIEF "3D graphics rendering library. Current implementation supports GLES2.0. Plan to extend support for vulkan."
#)

