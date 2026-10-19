# © Joseph Cameron - All Rights Reserved

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/convention_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/null_conformance.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/null_context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/profile_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/text_modeler_test.cpp"

    LIBRARIES
        gdkgraphics
        gdkgraphics_conformance
)

get_filename_component(GDK_TEST_FONT
    "${CMAKE_CURRENT_LIST_DIR}/../data/assets/fonts/gdk-test-font.ttf" ABSOLUTE)

get_filename_component(GDK_TEST_FONT_COLLECTION
    "${CMAKE_CURRENT_LIST_DIR}/../data/assets/fonts/gdk-test-font.ttc" ABSOLUTE)

target_compile_definitions(${JFC_TEST_TARGET} PRIVATE GDK_TEST_FONT="${GDK_TEST_FONT}"
    GDK_TEST_FONT_COLLECTION="${GDK_TEST_FONT_COLLECTION}")
