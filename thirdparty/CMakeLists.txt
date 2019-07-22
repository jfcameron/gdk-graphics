# © 2018 Joseph Cameron - All Rights Reserved

set(dependencies
    "stb"
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-math")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-memory")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simple-glfw")

jfc_add_dependencies(${dependencies})

