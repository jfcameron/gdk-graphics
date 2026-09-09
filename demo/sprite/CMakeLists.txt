# © Joseph Cameron - All Rights Reserved

project(sprite_demo)

add_executable(sprite_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_include_directories(sprite_demo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../incubate")

target_link_libraries(sprite_demo PRIVATE gdkgraphics)

target_compile_features(sprite_demo PRIVATE cxx_std_20)

