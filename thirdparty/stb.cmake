# © 2018 Joseph Cameron - All Rights Reserved

# generates the source file for the [header-only] projects in stb. Adds only the *IMPLEMENTATION defines and copies only the headers 
# for the specific stb projects named in ARGV.
# for example: import_stb_projects("stb_image" "stb_perlin") will define implementation and copy headers to the workspace only 
# for the perlin and image stb projects.
function(import_stb_projects)
    foreach(_arg ${ARGV})
        string(TOUPPER ${_arg} _arg_upper)

        string(APPEND _stb_imp_file_contents "#define ${_arg_upper}_IMPLEMENTATION\n#include \"stb/${_arg}.h\"\n")
    endforeach()

    file(WRITE "${PROJECT_BINARY_DIR}/stb_implementation.c" "${_stb_imp_file_contents}")

    foreach(_arg ${ARGV})
        file(COPY "${CMAKE_CURRENT_LIST_DIR}/${JFC_DEPENDENCY_NAME}/${_arg}.h" 
            DESTINATION "${PROJECT_BINARY_DIR}/include/${JFC_DEPENDENCY_NAME}/")
    endforeach()

    add_library(${PROJECT_NAME} STATIC
        ${PROJECT_BINARY_DIR}/stb_implementation.c)

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_BINARY_DIR}/include/)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

    set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 90)

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>")
endfunction()

import_stb_projects(
    #[["stb"
    "stb_c_lexer"
    "stb_connected_components"
    "stb_divide"
    "stb_dxt"
    "stb_easy_font"
    "stb_herringbone_wang_tile"]]
    "stb_image"
    #[["stb_image_resize"
    "stb_image_write"
    "stb_leakcheck"
    "stb_perlin"
    "stb_rect_pack"
    "stb_sprintf"
    "stb_textedit"
    "stb_tilemap_editor"
    "stb_truetype"
    "stb_voxel_render"
    "stretchy_buffer"]]
)

jfc_set_dependency_symbols(
    INCLUDE_PATHS
        ${PROJECT_BINARY_DIR}/include

    LIBRARIES
        ${PROJECT_BINARY_DIR}/lib${JFC_DEPENDENCY_NAME}.a
)
