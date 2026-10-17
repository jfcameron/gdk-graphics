#ifndef GDK_OPENGL
#define GDK_OPENGL

#include <gdk/graphics/build_info.h>

#if defined JFC_TARGET_PLATFORM_Emscripten || defined JFC_TARGET_PLATFORM_Android
// OpenGL ES 2: WebGL 1 in a browser, GLES 2 on Android. What differs from desktop GL (a shader's
// #version and precision, extensions to ask for) is said once, as GDK_GRAPHICS_GLES
#include <GLES2/gl2.h>
#define GDK_GRAPHICS_GLES 1
#elif defined JFC_TARGET_PLATFORM_Darwin
#include <OpenGL/gl.h>
#elif defined JFC_TARGET_PLATFORM_Linux
#include <GL/glew.h>
#elif defined JFC_TARGET_PLATFORM_Windows
#define GLEW_STATIC 
#include <GL/glew.h>
#else
#error the current platform is not supported
#endif

#ifndef GDK_GRAPHICS_GLES
#define GDK_GRAPHICS_GLES 0
#endif

#endif
