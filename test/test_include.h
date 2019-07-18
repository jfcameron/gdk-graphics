#ifndef JFC_TEST_INCLUDE_H
#define JFC_TEST_INCLUDE_H

#include <gdkgraphics/buildinfo.h>

#include <stdexcept>

#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

static void initGL()
{        
    if (!glfwInit()) throw std::runtime_error(std::string("InitGL").append("/glfwInit failed"));

    glfwWindowHint(GLFW_RESIZABLE, true);

    if (GLFWwindow *pWindow = glfwCreateWindow(400, 300, "gdk window", nullptr, nullptr))
    {
        glfwMakeContextCurrent(pWindow);

        // Vsync controller. if not called, the interval is platform dependent. 0 is off. negative values allow for swapping even if the backbuffer arrives late (vendor extension dependent).
        glfwSwapInterval(-1); 

        glfwSwapBuffers(pWindow);

#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
        if (GLenum err = glewInit() != GLEW_OK) throw std::runtime_error(std::string("InitGL").append("/glewinit failed"));
#endif
    }
    else throw std::runtime_error(std::string("InitGL").append("/glfw window init failed"));
}

#endif
