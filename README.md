# gdk-graphics
A 3D rendering interface for games. This library defines the interface and provides a null implementation, for tests. Other implementations are provided by separate libraries.

API documentation: https://jfcameron.github.io/gdk-graphics/

Implementations:
- [gdk-graphics_webgles](https://github.com/jfcameron/gdk-graphics_webgles): OpenGL ES 2.0 and WebGL 1.0, on desktop, mobile and the browser

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library and its tests on linux using gcc
cmake --preset linux-gcc

# build the library and tests
cmake --build --preset linux-gcc

# run the tests
ctest --preset linux-gcc
```
See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `gdk::graphics` (and `gdk::graphics_data` alone for the data without the interface), or install it and `find_package(gdk-graphics)`. Positions, rotations and matrices are gdk-math's, `gdk::math::vector3<float>`, `gdk::math::quaternion<float>` and `gdk::math::matrix4x4<float>`.

Nothing is thread-safe: a context and everything made from it are used from one thread, and an implementation over a graphics API needs that API's context current there.

## Overview
### Entry point
A context represents an instance of the graphics system and owns the global state associated with that instance.

### The 3D scene
The library is structured around 3D scenes. Each scene contains any number of cameras and entities, and holds them weakly: what its owner lets go of leaves the scene. A camera represents a viewpoint in the 3D world, while an entity represents something that can be rendered in the scene.

### Profiles
A profile holds a context to a target's limits: the largest texture, whether textures may be any size, how many uniforms and attributes a shader may have. Hold a development build to the smallest device it will run on, and what would fail there throws on the development machine instead.

### Data
`gdk::graphics_data` is the data the interface draws, and decoders for it, usable without any context.

#### `texture_data`
The library consumes texture data as raw pixel data in formats such as 32-bit RGBA. It provides a PNG decoder in its `ext` headers, both as a convenience and as a reference for integrating other image decoders.

#### `model_data`
Vertex data is consumed as a list of floating-point values representing vertex attributes, indexed or not. Vertex data is used to construct models, which provide the geometry used to give entities their appearance in a scene. `ext` reads glTF documents into it.

#### Decoders
The decoders in `ext` read PNG, glTF, Aseprite sheets, BVH motion and TrueType or OpenType fonts. Each takes bytes, never a file name, and each is safe on a hostile file: what it cannot read is refused with `gdk::graphics::exception`, rather than read past or followed without end. Each has a fuzzer in `data/fuzz`.

### Text
`ext::text_modeler` lays a string out with a font's glyphs into a model, drawing from a texture of the font's atlas, which several modelers can share.

### Shaders
The library does not provide a mechanism for creating user-defined shaders. Shader languages, compilation mechanisms, and available capabilities vary between graphics APIs, making a unified shader interface either restrictive or complex.

Instead, user-defined shaders are created through the selected implementation library's interface. This allows users to take advantage of the capabilities and shading language of their chosen backend without requiring `gdk-graphics` to impose a common abstraction over languages such as GLSL and HLSL. For a concrete example of this, see the demos in [gdk-graphics_webgles](https://github.com/jfcameron/gdk-graphics_webgles), some of which compile custom shaders.

## Conformance
`gdkgraphics_conformance` is the suite every implementation passes. An implementation's test target links it and defines `gdk::graphics::conformance::make_context()`, `implementation()`, `minimum_profile()` and `checks_shaders()` (see `test/conformance/conformance.h`); the null implementation runs it here.

`gdkgraphics_implementation` gives an implementation the private helpers the null one uses too (the texture patch check). It is a build-time target and is never installed.

## Fuzzing
With clang, configure with `-DJFC_BUILD_FUZZERS=ON` and run a fuzzer from `data/fuzz` with a scratch corpus directory first and `data/fuzz/corpus/<format>` after it.
