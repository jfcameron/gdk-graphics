# © 2018 Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 17
    C_STANDARD 90

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/camera_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/color_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/floatuniformcollection_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mat4x4uniformcollection_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/model_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/shaderprogram_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/textureuniformcollection_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector2uniformcollection_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector3uniformcollection_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vector4uniformcollection_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vertexattribute_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vertexdata_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vertexformat_test.cpp"

        #"${CMAKE_CURRENT_LIST_DIR}/glh_test.cpp"

    INCLUDE_DIRECTORIES
        "${${PROJECT_NAME}_INCLUDE_DIRECTORIES}"

    LIBRARIES
        "${${PROJECT_NAME}_LIBRARIES}"
)

