# © 2018 Joseph Cameron - All Rights Reserved

set(dependencies
    "stb"

    "glfw" # some cleanup here gotta be done
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-math")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-memory")

jfc_add_dependencies(${dependencies})

