# © 2019 Joseph Cameron - All Rights Reserved

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/freetype")

jfc_set_dependency_symbols(
    INCLUDE_PATHS
        ${CMAKE_CURRENT_LIST_DIR}/freetype/include

        # Optional dependencies freetype can be configured to include
        ${HARFBUZZ_INCLUDE_DIRS}
        ${PNG_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}

        ${BZIP2_INCLUDE_DIRS}
        ${BZIP2_INCLUDE_DIR} #not documented but holds value on my linux machine
    
    LIBRARIES
        ${PROJECT_BINARY_DIR}/freetype/lib${JFC_DEPENDENCY_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
        
        # Optional dependencies freetype can be configured to include
        ${HARFBUZZ_LIBRARIES}
        ${PNG_LIBRARIES}
        ${ZLIB_LIBRARIES}

        ${BZIP2_LIBRARIES}


        ${BZIP2_LIBRARY_RELEASE} #not documented but holds val on my linux
        ${PNG_LIBRARY_RELEASE} #same
        ${ZLIB_LIBRARY_RELEASE} #TODO: write a fallback when relase exist but not LIBRARIES
)

#jfc_print_all_variables()
#message(FATAL_ERROR "LOL: ${HARFBUZZ_LIBRARIES}${BZip2_LIBRARIES}${BZIP2_INCLUDE_DIRS}")

