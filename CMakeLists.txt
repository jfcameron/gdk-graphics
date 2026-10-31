# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET gdkgraphics)
    return()
endif()

project(gdk-graphics
    VERSION 1.0
    DESCRIPTION "the gdk graphics interface: scenes, cameras, entities, models, materials and textures, a null implementation, and the data they are made from"
    LANGUAGES C CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(GDKGRAPHICS_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GDKGRAPHICS_BUILD_DOCS "Build documentation" OFF)

if (GDKGRAPHICS_BUILD_DOCS)
    jfc_generate_documentation_doxygen(
        README_PATH      README.md
        OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/docs

        INCLUDE_DIRECTORIES
            include/
            impl/null/include/
            data/include/
    )

    if (NOT GDKGRAPHICS_BUILD_TESTS)
        return()
    endif()
endif()

add_subdirectory(extern)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_subdirectory(data)

add_library(gdkgraphics STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/profile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/texture_patch.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/batch_modeler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/font_texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/text_modeler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/utf8.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/null/src/null_context.cpp")

add_library(gdk::graphics ALIAS gdkgraphics)

set_target_properties(gdkgraphics PROPERTIES EXPORT_NAME graphics PREFIX "lib")

target_include_directories(gdkgraphics
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

target_compile_features(gdkgraphics PUBLIC cxx_std_20)

target_link_libraries(gdkgraphics PUBLIC gdkgraphics_data gdkmath)

add_library(gdkgraphics_implementation INTERFACE)

target_include_directories(gdkgraphics_implementation INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

target_link_libraries(gdkgraphics_implementation INTERFACE gdkgraphics)

add_library(gdkgraphics_conformance INTERFACE)

target_sources(gdkgraphics_conformance INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/context_conformance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/profile_conformance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/text_conformance.cpp")

target_include_directories(gdkgraphics_conformance INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance")

jfc_embed_file(GDK_TEST_FONT_BYTES "${CMAKE_CURRENT_SOURCE_DIR}/data/assets/fonts/gdk-test-font.ttf"
    "${PROJECT_BINARY_DIR}/test_fonts/gdk/graphics/test/font_bytes.h")

jfc_embed_file(GDK_TEST_FONT_COLLECTION_BYTES "${CMAKE_CURRENT_SOURCE_DIR}/data/assets/fonts/gdk-test-font.ttc"
    "${PROJECT_BINARY_DIR}/test_fonts/gdk/graphics/test/font_collection_bytes.h")

target_include_directories(gdkgraphics_conformance INTERFACE "${PROJECT_BINARY_DIR}/test_fonts")

target_link_libraries(gdkgraphics_conformance INTERFACE gdkgraphics)

install(TARGETS gdkgraphics gdkgraphics_data
    EXPORT gdk-graphics-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(EXPORT gdk-graphics-targets
    FILE gdk-graphics-targets.cmake
    NAMESPACE gdk::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-graphics")

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/gdk-graphics-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

file(WRITE "${PROJECT_BINARY_DIR}/gdk-graphics-config.cmake"
"include(CMakeFindDependencyMacro)
find_dependency(gdk-math 1)
include(\"\${CMAKE_CURRENT_LIST_DIR}/gdk-graphics-targets.cmake\")
")

install(FILES
    "${PROJECT_BINARY_DIR}/gdk-graphics-config.cmake"
    "${PROJECT_BINARY_DIR}/gdk-graphics-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-graphics")

if (GDKGRAPHICS_BUILD_TESTS)
    add_subdirectory(test)
endif()
