// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SHADERPROGRAM_H
#define GDK_GFX_SHADERPROGRAM_H

#include <gdk/opengl.h>
#include <jfc/lazy_ptr.h>
#include <jfc/unique_handle.h>

#include <string>

namespace gdk
{
    /// \brief Specifies drawing behaviours at the two programmable stages in the OpenGL ES 2.0/WebGL 1.0 pipeline 
    /// (the vertex shader stage and fragment shader stage)
    //TODO: consider a "pipeline" abstraction, which specifies all fixed options, then takes a program to specify programmable ones.
    //TODO: pipline abstraction is required to properly support blend-based transparencies, since frag order matters when blending.
    class shader_program final
    {
    public:
        //! specify whether front- or back-facing polygons can be culled
        enum class FaceCullingMode 
        {
            //! cull front facing polygons
            Front,

            //! cull back facing polygons
            Back,

            //! cull front and back facing polygons
            FrontAndBack,
            
            //! do not cull any polygons
            None
        };

    private:
        //! handle to the program in the gl context.
        jfc::unique_handle<GLuint> m_ProgramHandle; 

        //! Whether or not to discard polygons based on [model space] normal direction
        FaceCullingMode m_FaceCullingMode = FaceCullingMode::None;
        
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

        //! shader useful for indicating some kind of failure. Does MVP mul then colors all frags a bright pink.
        static const jfc::lazy_ptr<gdk::shader_program> PinkShaderOfDeath;

        //! shader for drawing unlit surfaces with alpha channel based fragment discard. Suitable for text rendering, 
        /// GUI element rendering, 2D Sprite rendering. Extremely lightweight.
        static const jfc::lazy_ptr<gdk::shader_program> AlphaCutOff;
    };
}

#endif
