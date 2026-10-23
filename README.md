# gdk-graphics
A 3D rendering interface for games. This library defines the interface and provides a null implementation. Other implementations are provided by separate libraries.

See [gdk-graphics_webgles](https://github.com/jfcameron/gdk-graphics_webgles) for the OpenGL ES 2.0 and WebGL 1.0 implementation.

## Overview
### Entry point
A context represents an instance of the graphics system and owns the global state associated with that instance.

### The 3D scene
The library is structured around 3D scenes. Each scene contains any number of cameras and entities. A camera represents a viewpoint in the 3D world, while an entity represents something that can be rendered in the scene.

### Data
#### `texture_data`
The library consumes texture data as raw pixel data in formats such as 32-bit RGBA. It provides a PNG decoder in its `ext` headers, both as a convenience and as a reference for integrating other image decoders.

#### `vertex_data`
Vertex data is consumed as a list of floating-point values representing vertex attributes. Vertex data is used to construct models, which provide the geometry used to give entities their appearance in a scene.

### Shaders
The library does not provide a mechanism for creating user-defined shaders. Shader languages, compilation mechanisms, and available capabilities vary between graphics APIs, making a unified shader interface either restrictive or complex.

Instead, user-defined shaders are created through the selected implementation library's interface. This allows users to take advantage of the capabilities and shading language of their chosen backend without requiring `gdk-graphics` to impose a common abstraction over languages such as GLSL and HLSL. For a concrete example of this, see the demos in [gdk-graphics_webgles](https://github.com/jfcameron/gdk-graphics_webgles), some of which compile custom shaders.

