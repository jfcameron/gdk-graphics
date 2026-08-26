#ifndef GDK_OPENGL
#define GDK_OPENGL

#include <gdkgraphics/buildinfo.h>

#ifdef JFC_TARGET_PLATFORM_Emscripten
#include <GLES2/gl2.h>
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

#endif
