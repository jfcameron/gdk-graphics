// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SHADER_PROGRAM_H
#define GDK_GFX_SHADER_PROGRAM_H

namespace gdk
{
    /// \brief Specifies drawing behaviours at the programmable stages in the graphics pipeline 
    class shader_program {
    public:
        virtual ~shader_program() = default;

    protected:
        shader_program() = default;
    };
}

#endif

