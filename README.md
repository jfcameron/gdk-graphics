[![Build Status](https://travis-ci.org/jfcameron/gdk-graphics.svg?branch=master)](https://travis-ci.org/jfcameron/gdk-graphics) [![Documentation](https://img.shields.io/badge/documentation-doxygen-blue.svg)](https://jfcameron.github.io/gdk-graphics/)

## gdk-graphics

3D graphics rendering library. Current implementation supports GLES2.0 & WebGL 1.0.

## CI & Documentation

Documentation generated with doxygen can be viewed online here: https://jfcameron.github.io/gdk-graphics/

Continuous integration builds are done for all PRs and all changes to main. Build scripts cover Windows, Linux, Mac, WebAssembly; Clang, GCC, MSVC; x86_64, arm64: https://travis-ci.org/jfcameron/gdk-graphics

catch2 unit tests available under `test/` and can be run with ctest

## Build instructions
Open the `.travis.yml` file on the root of the project, refer to the section for your platform

## Sample project

An example of a game project using this can be found here: https://github.com/jfcameron/flappy-dot

