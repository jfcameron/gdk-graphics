# © Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/camera_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/color_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/convention_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/entity_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/gltf_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/material_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/model_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/model_data_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/null_context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/string_keyed_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/scene_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/shader_program_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_include.h"

    LIBRARIES
        gdkgraphics
)

