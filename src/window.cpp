// © 2018-2019 Joseph Cameron - All Rights Reserved

#include <gdk/window.h>

#ifdef JFC_TARGET_PLATFORM_Emscripten
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#elif defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <exception>
#include <functional>
#include <iostream>
#include <sstream>

static constexpr char TAG[] = "SimpleGLFWWindow";

namespace gdk
{
    SimpleGLFWWindow::SimpleGLFWWindow(const std::string &aName)
    : m_pGLFWWindow([&]()
        {        
            glfwSetErrorCallback([](int, const char *msg)
            {
                throw std::runtime_error(std::string(TAG).append("/").append(msg));
            });
        
            if (!glfwInit()) throw std::runtime_error(std::string(TAG).append("/glfwInit failed"));
                
            //const gdk::IntVector2 aScreenSize(400,300);
            const SimpleGLFWWindow::window_size_type aScreenSize = std::make_pair(400,300);
        
            windowSize = aScreenSize;
        
            //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_RESIZABLE, true);

            if (GLFWwindow *pWindow = glfwCreateWindow(aScreenSize.first, aScreenSize.second, aName.c_str(), nullptr, nullptr))
            {
                glfwMakeContextCurrent(pWindow);

                // Vsync controller. if not called, the interval is platform dependent. 0 is off. negative values allow for swapping even if the backbuffer arrives late (vendor extension dependent).
                glfwSwapInterval(-1); 

                glfwSwapBuffers(pWindow); //Must be called once to see the buffer we cleared

                glfwPollEvents(); //Must be called once for window to render

                glfwSetWindowUserPointer(pWindow, static_cast<void *>(this));
                    
                glfwSetWindowSizeCallback(pWindow, [](GLFWwindow *const pCurrentGLFWwindow, int aX, int aY)
                {
                    if (SimpleGLFWWindow *const pCurrentWrapper = static_cast<SimpleGLFWWindow *const>(glfwGetWindowUserPointer(pCurrentGLFWwindow)))
                    {
                        pCurrentWrapper->windowSize.first =  aX;
                        pCurrentWrapper->windowSize.second = aY;
                    }
                    else throw std::runtime_error(std::string(TAG).append("/wrapper associated with current glfw window instance is null"));
                });

#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
                if (GLenum err = glewInit() != GLEW_OK) throw std::runtime_error(std::string(TAG).append("/glewinit failed: ").append(glewGetErrorString(err)));
#endif
                return pWindow;
            }
            else throw std::runtime_error(std::string(TAG).append("/glfwCreateWindow failed. Can the environment provide a GLES2.0/WebGL1.0 context?"));
        }()
        ,[](GLFWwindow *const ptr)
        {
            glfwDestroyWindow(ptr);
        })
        , m_Name(aName)
    {}

    GLFWwindow *const SimpleGLFWWindow::getPointerToGLFWWindow()
    {
        return m_pGLFWWindow.get();
    }

    void SimpleGLFWWindow::SwapBuffer()
    {
        glfwSwapBuffers(m_pGLFWWindow.get());
    }

    double SimpleGLFWWindow::GetTime()
    {
        return glfwGetTime();
    }

    SimpleGLFWWindow::window_size_type SimpleGLFWWindow::GetWindowSize()        
    {
        return windowSize;
    }

    bool SimpleGLFWWindow::GetMouseButton(const int aButton)
    {
        return glfwGetMouseButton(m_pGLFWWindow.get(), aButton);
    }

    SimpleGLFWWindow::cursor_position_type SimpleGLFWWindow::GetCursorPos()
    {
        double x,y;
        glfwGetCursorPos(m_pGLFWWindow.get(), &x, &y);

        return cursor_position_type(static_cast<float>(x), static_cast<float>(y));
    }

    bool SimpleGLFWWindow::GetKey(const int aKeyCode)
    {
        return static_cast<bool>(glfwGetKey(m_pGLFWWindow.get(), aKeyCode));
    }

    void SimpleGLFWWindow::pollEvents()
    {
        //TODO: call makecontextcurrent or soemthing

        glfwPollEvents();
    }

    std::string SimpleGLFWWindow::getName()
    {
        return m_Name;
    }
}

