# © Joseph Cameron - All Rights Reserved

set(dependencies
    "stb"
)

jfc_add_dependencies(${dependencies})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-math")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-memory")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-smart_handles")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simple-glfw")
