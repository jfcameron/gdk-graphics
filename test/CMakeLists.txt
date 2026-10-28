# © Joseph Cameron - MIT License

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/convention_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/null_conformance.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/null_context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/profile_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/text_modeler_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/utf8_test.cpp"

    LIBRARIES
        gdkgraphics
        gdkgraphics_conformance)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

set(_gdk_graphics_data_tests "${PROJECT_SOURCE_DIR}/data/test")

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${_gdk_graphics_data_tests}/animation_test.cpp"
        "${_gdk_graphics_data_tests}/aseprite_test.cpp"
        "${_gdk_graphics_data_tests}/color_test.cpp"
        "${_gdk_graphics_data_tests}/font_test.cpp"
        "${_gdk_graphics_data_tests}/gltf_test.cpp"
        "${_gdk_graphics_data_tests}/model_data_test.cpp"
        "${_gdk_graphics_data_tests}/png_test.cpp"
        "${_gdk_graphics_data_tests}/sprite_animation_test.cpp"
        "${_gdk_graphics_data_tests}/string_keyed_test.cpp"

    LIBRARIES
        gdkgraphics_data)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

target_include_directories(${JFC_TEST_TARGET} PRIVATE "${PROJECT_BINARY_DIR}/test_fonts")

if (NOT CMAKE_CROSSCOMPILING)
    add_test(NAME gdk-graphics_install_and_consume
        COMMAND "${CMAKE_COMMAND}"
            "-DBUILD_DIR=${PROJECT_BINARY_DIR}"
            "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_and_consume"
            "-DCONFIG=$<CONFIG>"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DPLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DEXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
            -P "${CMAKE_CURRENT_LIST_DIR}/install/install_and_consume.cmake")
endif()
