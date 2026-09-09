# © Joseph Cameron - All Rights Reserved

project(translucency_demo)

add_executable(translucency_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(translucency_demo PRIVATE gdkgraphics)

target_compile_features(translucency_demo PRIVATE cxx_std_20)

