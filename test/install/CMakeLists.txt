# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(gdk-graphics_consumer LANGUAGES CXX)

find_package(gdk-graphics 1 REQUIRED)

add_executable(consumer main.cpp)

target_link_libraries(consumer PRIVATE gdk::graphics)

set_target_properties(consumer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<0:>")
