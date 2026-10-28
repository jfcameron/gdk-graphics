# © Joseph Cameron - MIT License

foreach (_gdk_graphics_fuzzer aseprite bvh font gltf png)
    jfc_add_fuzz_tests(
        C++_STANDARD 20

        FUZZ_SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/${_gdk_graphics_fuzzer}_fuzz.cpp"

        CORPUS_DIRECTORIES
            "${CMAKE_CURRENT_LIST_DIR}/corpus/${_gdk_graphics_fuzzer}"

        LIBRARIES
            gdkgraphics_data
    )
endforeach()
