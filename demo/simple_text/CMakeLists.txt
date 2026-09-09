# © Joseph Cameron - All Rights Reserved

project(simple_text_demo)

add_executable(simple_text_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/text_modeler.cpp)

target_include_directories(simple_text_demo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../incubate")

target_link_libraries(simple_text_demo PRIVATE gdkgraphics)

target_compile_features(simple_text_demo PRIVATE cxx_std_20)

