// © 2019 Joseph Cameron - All Rights Reserved

#include <memory>

#ifndef GDK_GFX_CONTEXT_H
#define GDK_GFX_CONTEXT_H

namespace gdk::graphics
{
    //! entry point for gdk-graphics
    /// the context is responsible for establishing a context on the graphical hardware,
    /// creating a rendering environment (a window, in a desktop environment)
    /// constructing graphics types (shader, model, etc.)
    class context
    {
    public:
        //! ptr type returned by factory method
        using context_ptr_type = std::unique_ptr<context>;

        //! specifies implementation to use in context construction
        enum class implementation
        {
            //! limited subset of OpenGL API available on a wide variety of desktop, 
            /// mobile devices and browsers
            opengl_webgl1_gles2
        };

        //! factory method
        static context_ptr_type make(const implementation &);

        //! virtual destructor
        virtual ~context() = default;

    protected:
        //! prevent instantiation
        context() = default;
    };
}

#endif
