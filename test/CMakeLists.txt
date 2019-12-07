# © 2018 Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 17
    C_STANDARD 90

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/camera_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/color_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/entity_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/material_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/model_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/scene_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/shader_program_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test_include.h"
        "${CMAKE_CURRENT_LIST_DIR}/texture_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vertex_attribute_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vertex_format_test.cpp"

        #"${CMAKE_CURRENT_LIST_DIR}/glh_test.cpp"

    INCLUDE_DIRECTORIES
        "${${PROJECT_NAME}_INCLUDE_DIRECTORIES}"

    LIBRARIES
        "${${PROJECT_NAME}_LIBRARIES}"
)

