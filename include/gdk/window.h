// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_SIMPLE_GLFW_WINDOW_H
#define GDK_SIMPLE_GLFW_WINDOW_H

#include <functional>
#include <memory>
#include <string>
#include <utility>

struct GLFWwindow;

namespace gdk
{
    /// \brief RAII wrapper for GLFW, with simplified interface to GLFW
    /// \todo hide init and terminate inside of the constructor and destructor
    /// \todo pWindow should be a shared_ptr, the shared_ptr should clean the ptr. getPointer should return a weak_ptr
    class SimpleGLFWWindow final
    {
    public:
        using window_size_type = std::pair<int, int>;
        using cursor_position_type = std::pair<float, float>;

    private:
        //glfw::windowsizecallback_type WindowSizeCallback;
        
        window_size_type windowSize = {};

        std::shared_ptr<GLFWwindow> m_pGLFWWindow;

        std::string m_Name;

    public:
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //! Get the amount of time since the window was created
        /// \WARN What is the unit?
        static double GetTime();

        //! Causes GLFW to update event states
        static void PollEvents();

        //! get direct access to the GLFW pointer, primarily to allow GLFW dependant projects to work alongside this wrapper.
        GLFWwindow *const getPointerToGLFWWindow();

        //! Swap the front buffer (currently presented on display) with the backbuffer (currently being written to)
        void SwapBuffer();

        //! Get size in pixels of the window
        window_size_type GetWindowSize();

        std::string getName();

        //! Attach a callback to the window which will be called whenver the window is resized and provides that size info.
        /// \WARN You can only attach one function at a time.
        /// \WARN Should this support an arbitray number of callbacks?
        //void SetWindowSizeCallback(windowsizecallback_type aWindowSizeCallback);
        
        //! Get state of button by index
        bool GetMouseButton(const int aButton);

        //! Get position of the mouse cursor
        cursor_position_type GetCursorPos();

        //! Get a key value
        bool GetKey(const int aKeyCode);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SimpleGLFWWindow(const std::string &aName = "GDK Window");
    };
}

#endif
