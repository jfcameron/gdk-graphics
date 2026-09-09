# © Joseph Cameron - All Rights Reserved

function(import_cgltf_projects)
    foreach(_arg ${ARGV})
        string(TOUPPER ${_arg} _arg_upper)

        string(APPEND _cgltf_imp_file_contents "#define ${_arg_upper}_IMPLEMENTATION\n#include \"cgltf/${_arg}.h\"\n")
    endforeach()

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/cgltf_implementation.c" "${_cgltf_imp_file_contents}")

    foreach(_arg ${ARGV})
        file(COPY "${CMAKE_CURRENT_LIST_DIR}/cgltf/${_arg}.h"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/cgltf/")
    endforeach()

    add_library(cgltf STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/cgltf_implementation.c)

    target_include_directories(cgltf PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

    set_property(TARGET cgltf PROPERTY C_STANDARD 99)

    set_target_properties(cgltf PROPERTIES PREFIX "lib")
endfunction()

import_cgltf_projects(
    "cgltf"
)
