# © Joseph Cameron - All Rights Reserved

include("${CMAKE_CURRENT_LIST_DIR}/stb.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cgltf.cmake")

set(_jfc_outer_build_tests "${JFC_BUILD_TESTS}")
set(_jfc_outer_build_docs "${JFC_BUILD_DOCS}")
set(_jfc_outer_build_demo "${JFC_BUILD_DEMO}")
set(_jfc_outer_build_bench "${JFC_BUILD_BENCH}")

set(JFC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_DEMO OFF CACHE BOOL "" FORCE)
set(JFC_BUILD_BENCH OFF CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-math")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-time")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-memory")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/jfc-smart_handles")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gdk-windowing")

set(JFC_BUILD_TESTS "${_jfc_outer_build_tests}" CACHE BOOL "" FORCE)
set(JFC_BUILD_DOCS "${_jfc_outer_build_docs}" CACHE BOOL "" FORCE)
set(JFC_BUILD_DEMO "${_jfc_outer_build_demo}" CACHE BOOL "" FORCE)
set(JFC_BUILD_BENCH "${_jfc_outer_build_bench}" CACHE BOOL "" FORCE)
