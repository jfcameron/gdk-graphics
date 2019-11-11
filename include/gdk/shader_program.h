// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SHADERPROGRAM_H
#define GDK_GFX_SHADERPROGRAM_H

#include <gdk/opengl.h>
#include <jfc/lazy_ptr.h>
#include <jfc/unique_handle.h>

#include <string>

namespace gdk
{
    /// \brief Specifies drawing behaviours at [at least 2] of the programmable stages in the OpenGL pipeline.
    class shader_program final
    {
        jfc::unique_handle<GLuint> m_ProgramHandle; //!< handle to the program in the context.
        
    public:
        //! Installs this program's shaders to their corresponding programmable stages 
        /// will be used for subsequent draw calls until a different program is installed.
        GLuint useProgram() const;
                
        /// \brief equality semantics
        bool operator==(const shader_program &) const; 
        /// \brief equality semantics
        bool operator!=(const shader_program &) const; 

        /// \brief move semantics
        shader_program(shader_program &&) = default;
        /// \brief move semantics
        shader_program &operator=(shader_program &&) = default;
        
        /// \brief constructs a shader program with glsl source file contents for a vertex shader and a fragment shader 
        shader_program(std::string aVertexSource, std::string aFragmentSource);

        static const jfc::lazy_ptr<gdk::shader_program> PinkShaderOfDeath; //!< shader for indicating some kind of failure
        static const jfc::lazy_ptr<gdk::shader_program> AlphaCutOff;       //!< shader for drawing unlit surfaces with alpha channel based transparency
    };
}

#endif
