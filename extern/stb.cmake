# © Joseph Cameron - All Rights Reserved

function(import_stb_projects)
    foreach(_arg ${ARGV})
        string(TOUPPER ${_arg} _arg_upper)
        string(APPEND _stb_imp_file_contents "#define ${_arg_upper}_IMPLEMENTATION\n#include \"stb/${_arg}.h\"\n")
    endforeach()

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/stb_implementation.c" "${_stb_imp_file_contents}")

    foreach(_arg ${ARGV})
        file(COPY "${CMAKE_CURRENT_LIST_DIR}/stb/${_arg}.h" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/stb/")
    endforeach()

    add_library(stb STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/stb_implementation.c)

    target_include_directories(stb PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

    set_property(TARGET stb PROPERTY C_STANDARD 99)
    set_property(TARGET stb PROPERTY C_STANDARD_REQUIRED ON)
    set_property(TARGET stb PROPERTY C_EXTENSIONS OFF)

    set_target_properties(stb PROPERTIES PREFIX "lib")
endfunction()

import_stb_projects(
    "stb_image"
    "stb_rect_pack"
    "stb_truetype"
)
