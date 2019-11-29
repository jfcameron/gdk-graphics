// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MATERIAL_H
#define GDK_GFX_MATERIAL_H

#include <gdk/shader_program.h>

namespace gdk
{
    //! bag of uniform state, opengl pipeline state, and the current shader program to use
    class material
    {
    public:

    private:
        //! modifies the opengl state, assigning the program, assigning values to the program's uniforms etc.
        void activate();
    };
}

#endif
