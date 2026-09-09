# © Joseph Cameron - All Rights Reserved

function(import_stb_projects)
    foreach(_arg ${ARGV})
        string(TOUPPER ${_arg} _arg_upper)

        string(APPEND _stb_imp_file_contents "#define ${_arg_upper}_IMPLEMENTATION\n#include \"stb/${_arg}.h\"\n")
    endforeach()

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/stb_implementation.c" "${_stb_imp_file_contents}")

    foreach(_arg ${ARGV})
        file(COPY "${CMAKE_CURRENT_LIST_DIR}/stb/${_arg}.h"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/stb/")
    endforeach()

    add_library(stb STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/stb_implementation.c)

    target_include_directories(stb PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

    set_property(TARGET stb PROPERTY C_STANDARD 90)

    set_target_properties(stb PROPERTIES PREFIX "lib")
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
