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
    if (!glfwInit()) throw std::runtime_error(std::string(TAG).append("/glfwInit failed"));

    glfwWindowHint(GLFW_RESIZABLE, true);

    if (GLFWwindow *pWindow = glfwCreateWindow(400, 300, "gdk window", nullptr, nullptr))
    {
        glfwMakeContextCurrent(pWindow);

        // Vsync controller. if not called, the interval is platform dependent. 0 is off. negative values allow for swapping even if the backbuffer arrives late (vendor extension dependent).
        glfwSwapInterval(-1); 

        glfwSwapBuffers(pWindow);

#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
        if (GLenum err = glewInit() != GLEW_OK) throw std::runtime_error(std::string(TAG).append("/glewinit failed"));
#endif
    }
    else throw std::runtime_error(std::string(TAG).append("/glfw window init failed"));
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

