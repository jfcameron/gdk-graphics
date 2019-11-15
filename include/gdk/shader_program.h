// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SHADERPROGRAM_H
#define GDK_GFX_SHADERPROGRAM_H

#include <gdk/opengl.h>
#include <gdk/texture.h>
#include <gdk/graphics_types.h>
#include <jfc/lazy_ptr.h>
#include <jfc/unique_handle.h>

#include <array>
#include <string>
#include <unordered_map>

namespace gdk
{
    /// \brief Specifies drawing behaviours at the two programmable stages in the OpenGL ES 2.0/WebGL 1.0 pipeline 
    /// (the vertex shader stage and fragment shader stage)
    //TODO: consider a "pipeline" abstraction, which specifies all fixed options, then takes a program to specify programmable ones.
    //TODO: pipline abstraction is required to properly support blend-based transparencies, since frag order matters when blending.
    class shader_program final
    {
    public:
        /// \brief index, size, type of an active attribute used in the shader program
        struct active_attribute_info
        {
            //! location of the attribute
            GLint location;

            //! type of the attribute's components, e.g: float
            GLenum type;

            //! number of components in the attribute, e.g: 2
            GLint count; //TODO this is not component count. this is number of types (vec2 is 1 not 2)
        };

        /// \brief index. size. type o an active uniform used in the shader program
        struct active_uniform_info
        {
            //! location of the uniform  e.g: 1
            GLint location;

            //! type of the uniform e.g: Texture
            GLenum type;

            //! size of the uniform e.g: 1
            GLint size;
        };

        using active_attribute_collection_type = std::unordered_map<std::string, active_attribute_info>;
        using active_uniform_collection_type = std::unordered_map<std::string, active_uniform_info>;

        //! specify whether front- or back-facing polygons can be culled
        // TODO: Does this belong to shader or to a separate "pipline" abstraction? Im not sure. Answer has to do with how strongly associated I believe culmode is to userdefined fragment stage is?
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

    public://private: //TODO fix access specifier

        //! handle to the  vertex shader
        jfc::unique_handle<GLuint> m_VertexShaderHandle;

        //! handle to the fragment shader
        jfc::unique_handle<GLuint> m_FragmentShaderHandle;

        //! handle to the program in the gl context.
        jfc::unique_handle<GLuint> m_ProgramHandle; 

        /// \brief attrib name to metadata
        active_attribute_collection_type m_ActiveAttributes;
        
        /// \brief uniform name to metadata
        active_uniform_collection_type m_ActiveUniforms;

        //! Whether or not to discard polygons based on [model space] normal direction
        FaceCullingMode m_FaceCullingMode = FaceCullingMode::None;

        
    public:
        using integer2_uniform_type = std::array<GLint, 2>;
        using integer3_uniform_type = std::array<GLint, 3>;
        using integer4_uniform_type = std::array<GLint, 4>;

        //! assign a float1 uniform from a float
        void setUniform(const std::string &aName, const GLfloat aValue) const;
        //! assign a float2 uniform from a 2 component vector
        void setUniform(const std::string &aName, const graphics_vector2_type &aValue) const;
        //! assign a float3 uniform from a 3 component vector
        void setUniform(const std::string &aName, const graphics_vector3_type &aValue) const;
        //! assign a float4 uniform from a 4 component vector
        void setUniform(const std::string &aName, const graphics_vector4_type &aValue) const;

        //! assign a float uniform array from an array of floats
        void setUniform(const std::string &aName, const std::vector<GLfloat> &avalue) const;
        //! assign a float2 uniform array from an array of float2s
        void setUniform(const std::string &aName, const std::vector<graphics_vector2_type> &avalue) const; 
        //! assign a value to a float3 uniform array from a vector of float3s
        void setUniform(const std::string &aName, const std::vector<graphics_vector3_type> &avalue) const;
        //! assign a value to a float4 uniform array from a vector of float4s
        void setUniform(const std::string &aName, const std::vector<graphics_vector4_type> &avalue) const; 

        //! assign a value to a integer uniform
        void setUniform(const std::string &aName, const GLint aValue) const;
        //! assign a value to a integer2 uniform
        void setUniform(const std::string aName, const integer2_uniform_type &aValue) const;
        //! assign a value to a integer3 uniform
        void setUniform(const std::string aName, const integer3_uniform_type &aValue) const;
        //! assign a value to a integer4 uniform
        void setUniform(const std::string aName, const integer4_uniform_type &aValue) const;

        //! assign a value to a integer uniform array
        void setUniform(const std::string &aName, const std::vector<GLint> &aValue) const;
        //! assign a value to a integer2 uniform array
        void setUniform(const std::string &aName, const std::vector<integer2_uniform_type> &aValue) const;
        //! assign a value to a integer3 uniform array
        void setUniform(const std::string &aName, const std::vector<integer3_uniform_type> &aValue) const;
        //! assign a value to a integer4 uniform array
        void setUniform(const std::string &aName, const std::vector<integer4_uniform_type> &aValue) const;

        //! assign a value to a bool uniform 
        //void setUniform(const std::string &aName, const bool &aValue) const;
        //! assign a value to a bvec2 uniform
        //void setUniform(const std::string &aName, const boolean2_uniform_type &a) const;
        //! assign a value to a bvec3 uniform
        //void setUniform(const std::string &aName, const boolean3_uniform_type &a) const;
        //! assign a value to a bvec4 uniform
        //void setUniform(const std::string &aName, const boolean4_uniform_type &a) const;

        //! assign a value to a bool uniform array 
        //void setUniform(const std::string &aName, const std::vector<bool> &aValue) const;
        //! assign a value to a bvec2 uniform array
        //void setUniform(const std::string>&aName, const std::vector<boolean2_uniform_type> &a) const;
        //! assign a value to a bvec3 uniform array
        //void setUniform(const std::string>&aName, const std::vector<boolean3_uniform_type> &a) const;
        //! assign a value to a bvec4 uniform array
        //void setUniform(const std::string>&aName, const std::vector<boolean4_uniform_type> &a) const;

        /*//! assign a mat2x2 uniform from a mat2x2
        void setUniform(const std::string &aName, const graphics_mat2x2_type &avalue) const; 
        //! assign a mat2x2 uniform array from a vector of mat2x2s
        void setUniform(const std::string &aName, const std::vector<graphics_mat2x2_type> &avalue) const; 

        //! assign a mat3x3 uniform from a mat3x3
        void setUniform(const std::string &aName, const graphics_mat3x3_type &avalue) const; 
        //! assign a mat3x3 uniform array from a vector of mat3x3s
        void setUniform(const std::string &aName, const std::vector<graphics_mat3x3_type> &avalue) const;*/

        //! assign a mat4x4 uniform from a mat4x4
        void setUniform(const std::string &aName, const graphics_mat4x4_type &avalue) const; 
        //! assign a mat4x4 uniform array from a vector of mat4x4s
        void setUniform(const std::string &aName, const std::vector<graphics_mat4x4_type> &avalue) const; 

        //TODO: texture needs to support more than tex2d!
        //! bind a texture to the context then assign it to a texture uniform
        void setUniform(const std::string &aName, const gdk::texture &aTexture) const;

        //TODO: texture needs to support more than tex2d!
        //! bind an array of textures to the context then assign them to texture uniforms
        //void setUniform(const std::string &aName, const gdk::texture &aTexture) const;

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

        //! shader useful for indicating some kind of failure. Performs MVP mul then colors all frags bright pink.
        static const jfc::lazy_ptr<gdk::shader_program> PinkShaderOfDeath;

        //! shader for drawing unlit surfaces with alpha channel based fragment discard. Suitable for text rendering, 
        /// GUI element rendering, 2D Sprite rendering. Extremely lightweight.
        static const jfc::lazy_ptr<gdk::shader_program> AlphaCutOff;
    };
}

#endif
