[![Build Status](https://travis-ci.org/jfcameron/gdk-graphics.svg?branch=master)](https://travis-ci.org/jfcameron/gdk-graphics) [![Coverage Status](https://coveralls.io/repos/github/jfcameron/gdk-graphics/badge.svg?branch=master)](https://coveralls.io/github/jfcameron/gdk-graphics?branch=master) [![Documentation](https://img.shields.io/badge/documentation-doxygen-blue.svg)](https://jfcameron.github.io/gdk-graphics/)

## gdk-graphics

3D graphics rendering library. Current implementation supports GLES2.0 & WebGL 1.0

Currently undergoing an overhaul to make the impl more sympathetic to OpenGL (reducing state changes, API calls as much as possible) 

## CI & Documentation

Documentation generated with doxygen ca be viewed online here: https://jfcameron.github.io/gdk-graphics/

Coverage calculated with gcov viewable here: https://coveralls.io/github/jfcameron/gdk-graphics

CI done using Travis CI. Build scripts cover Windows, Linux, Mac; Clang, GCC, MSVC, MinGW: https://travis-ci.org/jfcameron/gdk-graphics

catch2 unit tests available under `test/`.

