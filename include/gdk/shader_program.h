// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SHADERPROGRAM_H
#define GDK_GFX_SHADERPROGRAM_H

#include <gdk/opengl.h>
#include <jfc/lazy_ptr.h>

#include <iosfwd>
#include <string>

namespace gdk
{
    /// \brief Specifies drawing behaviours at [at least 2] of the programmable stages in the OpenGL programmable pipeline.
    ///
    /// \detailed none.
    class shader_program final
    {
        friend std::ostream &operator<< (std::ostream &, const shader_program &);
        
        std::string m_Name;      //!< Human friendly identifier        
        GLuint m_ProgramHandle = {0}; //!< handle to the program in the context.
        
    public:
        //! Installs the program to the pipeline. This program's programmable stages
        /// will be used for subsequent draw calls until a different program is installed.
        GLuint useProgram() const;
        
        std::string getName() const;
        GLuint getHandle() const;
                
        shader_program &operator=(const shader_program &) = delete;
        shader_program &operator=(shader_program &&) = delete;
                
        shader_program() = delete;
        shader_program(const std::string &aName, std::string aVertexSource, std::string aFragmentSource);
        shader_program(const shader_program &) = delete;
        shader_program(shader_program &&);
        ~shader_program();
        
        static const jfc::lazy_ptr<gdk::shader_program> PinkShaderOfDeath; //!< shader for indicating some kind of failure
        static const jfc::lazy_ptr<gdk::shader_program> AlphaCutOff;       //!< shader for drawing unlit surfaces with alpha channel based transparency
    };

    std::ostream &operator<< (std::ostream &, const shader_program &);
}

#endif
