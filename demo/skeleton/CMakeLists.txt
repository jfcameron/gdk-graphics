# © Joseph Cameron - All Rights Reserved

project(skeleton_demo)

add_executable(skeleton_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/content.cpp)

target_include_directories(skeleton_demo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../incubate")

target_link_libraries(skeleton_demo PRIVATE gdkgraphics)

target_compile_features(skeleton_demo PRIVATE cxx_std_20)
