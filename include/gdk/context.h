// © 2019 Joseph Cameron - All Rights Reserved

#include <memory>

#ifndef GDK_GFX_CONTEXT_H
#define GDK_GFX_CONTEXT_H

namespace gdk::graphics
{
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

    private:
        context() = delete;

        ~context() = default;
    };
}

#endif
