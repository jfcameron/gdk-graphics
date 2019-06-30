// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <gdk/shaderprogram.h>

using namespace gdk;

const std::string TAG("TEST");

void initGL()
{        
    /*glfwSetErrorCallback([](int, const char *msg)
            {
            throw std::runtime_error(std::string(TAG).append("/").append(msg));
            });*/

    if (!glfwInit()) throw std::runtime_error(std::string(TAG).append("/glfwInit failed"));

    //const gdk::IntVector2 aScreenSize(400,300);
    //const SimpleGLFWWindow::window_size_type aScreenSize = std::make_pair(400,300);

    //windowSize = aScreenSize;

    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, true);

    if (GLFWwindow *pWindow = glfwCreateWindow(400, 300, "aName.c_str()", nullptr, nullptr))
    {
        glfwMakeContextCurrent(pWindow);

        // Vsync controller. if not called, the interval is platform dependent. 0 is off. negative values allow for swapping even if the backbuffer arrives late (vendor extension dependent).
        glfwSwapInterval(-1); 

        glfwSwapBuffers(pWindow);

        //glfwSetWindowUserPointer(pWindow, static_cast<void *>(this));

#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
        if (GLenum err = glewInit() != GLEW_OK) throw std::runtime_error(std::string(TAG).append("/glewinit failed: "));//.append(glewGetErrorString(err)));
#endif
        //return pWindow;
    }
    //else throw std::runtime_error(std::string(TAG).append("/glfwCreateWindow failed. Can the environment provide a GLES2.0/WebGL1.0 context?"));
}


TEST_CASE("shaderprogram constructors", "[shaderprogram]" )
{
    SECTION("Default constructor produces an opaque black shaderprogram")
    {
        initGL();
        //auto pShader = ShaderProgram::AlphaCutOff;

        auto ptr = static_cast<std::shared_ptr<ShaderProgram>>(ShaderProgram::AlphaCutOff);

        //REQUIRE(shaderprogram == Color::Black);
    }
}

