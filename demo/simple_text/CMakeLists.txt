# © Joseph Cameron - All Rights Reserved

project(simple_text_demo)

jfc_embed_file("EMBEDDED_FONT"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../extern/gdk-graphics_data/assets/fonts/LiberationSans-Regular.ttf"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/embedded_font.h"
)

add_executable(simple_text_demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(simple_text_demo 
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../incubate"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

target_link_libraries(simple_text_demo PRIVATE gdkgraphics)

target_compile_features(simple_text_demo PRIVATE cxx_std_20)

