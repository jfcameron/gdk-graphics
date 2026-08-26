# © Joseph Cameron - All Rights Reserved

function(import_cgltf_projects)
    foreach(_arg ${ARGV})
        string(TOUPPER ${_arg} _arg_upper)

        string(APPEND _cgltf_imp_file_contents "#define ${_arg_upper}_IMPLEMENTATION\n#include \"cgltf/${_arg}.h\"\n")
    endforeach()

    file(WRITE "${PROJECT_BINARY_DIR}/cgltf_implementation.c" "${_cgltf_imp_file_contents}")

    foreach(_arg ${ARGV})
        file(COPY "${CMAKE_CURRENT_LIST_DIR}/${JFC_DEPENDENCY_NAME}/${_arg}.h"
            DESTINATION "${PROJECT_BINARY_DIR}/include/${JFC_DEPENDENCY_NAME}/")
    endforeach()

    add_library(${PROJECT_NAME} STATIC
        ${PROJECT_BINARY_DIR}/cgltf_implementation.c)

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_BINARY_DIR}/include/)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

    set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)

    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>")
endfunction()

import_cgltf_projects(
    "cgltf"
)

jfc_set_dependency_symbols(
    INCLUDE_PATHS
        ${PROJECT_BINARY_DIR}/include

    LIBRARIES
        ${PROJECT_BINARY_DIR}/lib${JFC_DEPENDENCY_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
)
