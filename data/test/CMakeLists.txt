# © Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/aseprite_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/color_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/font_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/gltf_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/model_data_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/sprite_animation_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/string_keyed_test.cpp"

    LIBRARIES
        gdkgraphics_data
)

get_filename_component(GDK_TEST_FONT
    "${CMAKE_CURRENT_LIST_DIR}/../assets/fonts/gdk-test-font.ttf" ABSOLUTE)

get_filename_component(GDK_TEST_FONT_COLLECTION
    "${CMAKE_CURRENT_LIST_DIR}/../assets/fonts/gdk-test-font.ttc" ABSOLUTE)

target_compile_definitions(${JFC_TEST_TARGET} PRIVATE GDK_TEST_FONT="${GDK_TEST_FONT}"
    GDK_TEST_FONT_COLLECTION="${GDK_TEST_FONT_COLLECTION}")
