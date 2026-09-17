# © Joseph Cameron - All Rights Reserved

jfc_add_fuzz_tests(
    C++_STANDARD 20

    FUZZ_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/gltf_fuzz.cpp"

    CORPUS_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/corpus/gltf"

    LIBRARIES
        gdkgraphics
)

jfc_add_fuzz_tests(
    C++_STANDARD 20

    FUZZ_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/aseprite_fuzz.cpp"

    CORPUS_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/corpus/aseprite"

    LIBRARIES
        gdkgraphics
)
