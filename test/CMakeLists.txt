# © Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/camera_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/convention_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/entity_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/material_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/model_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/null_context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/scene_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/shader_program_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_include.h"
        "${CMAKE_CURRENT_LIST_DIR}/text_modeler_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture3d_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture_test.cpp"

    LIBRARIES
        gdkgraphics
)

get_filename_component(GDK_TEST_FONT
    "${CMAKE_CURRENT_LIST_DIR}/../extern/gdk-graphics_data/assets/fonts/gdk-test-font.ttf" ABSOLUTE)

get_filename_component(GDK_TEST_FONT_COLLECTION
    "${CMAKE_CURRENT_LIST_DIR}/../extern/gdk-graphics_data/assets/fonts/gdk-test-font.ttc" ABSOLUTE)

target_compile_definitions(${JFC_TEST_TARGET} PRIVATE GDK_TEST_FONT="${GDK_TEST_FONT}"
    GDK_TEST_FONT_COLLECTION="${GDK_TEST_FONT_COLLECTION}")
