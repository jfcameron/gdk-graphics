// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_SHADER_PROGRAM_H
#define GDK_GFX_WEBGL1ES2_SHADER_PROGRAM_H

#include <gdk/graphics_types.h>
#include <gdk/opengl.h>
#include <gdk/shader_program.h>
#include <gdk/webgl1es2_texture.h>
#include <jfc/shared_proxy_ptr.h>
#include <jfc/unique_handle.h>

#include <array>
#include <optional>
#include <string>
#include <unordered_map>

namespace gdk
{
    /// \brief Specifies drawing behaviours at the two programmable stages in the OpenGL ES 2.0/WebGL 1.0 pipeline 
    /// (the vertex shader stage and fragment shader stage)
    //TODO: consider a "pipeline" abstraction, which specifies all fixed options, then takes a program to specify programmable ones.
    //TODO: pipline abstraction is required to properly support blend-based transparencies, since frag order matters when blending.
    class webgl1es2_shader_program final : public shader_program
    {
    public:
        /// \brief index, size, type of an active attribute used in the shader program
        struct active_attribute_info
        {
            GLint location; //!< location of the attribute
            GLenum type; //!< type of the attribute's components, e.g: float
            //TODO this is not component count. this is number of types (vec2 is 1 not 2)
            GLint count; //!< number of components in the attribute, e.g: 2
        };

        /// \brief index. size. type o an active uniform used in the shader program
        struct active_uniform_info
        {
            GLint location; //!< location of the uniform  e.g: 1
            GLenum type; //!< type of the uniform e.g: Texture
            GLint size; //!< size of the uniform e.g: 1
        };

        //! associative collection which maps an active attribute by its name to its index in the program. used in memoization strategy to reduce opengl api calls
        using active_attribute_collection_type = std::unordered_map<std::string, active_attribute_info>;
        //! associative collection which maps an active uniform by its name to its index in the program. used in memoization strategy to reduce opengl api calls
        using active_uniform_collection_type = std::unordered_map<std::string, active_uniform_info>;

        //! specify whether front- or back-facing polygons can be culled
        // TODO: Does this belong to shader or to a separate "pipline" abstraction? Im not sure. Answer has to do with how strongly associated I believe culmode is to userdefined fragment stage is?
        enum class FaceCullingMode 
        {
            Front, //!< cull front facing polygons
            Back, //!< cull back facing polygons
            FrontAndBack, //!< cull front and back facing polygons
            None //!< do not cull any polygons
        };
       
        //! type alias used for setting int2 uniform value
        using integer2_uniform_type = std::array<GLint, 2>;
        //! type alias used for setting int3 uniform value
        using integer3_uniform_type = std::array<GLint, 3>;
        //! type alias used for setting int4 uniform value
        using integer4_uniform_type = std::array<GLint, 4>;

    private:
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

        //! Whether or not to discard polygons based on [entity space] normal direction
        FaceCullingMode m_FaceCullingMode = FaceCullingMode::None;
        
    public:
        //! returns a nonnull optional to an attribute info if one with the given name exists
        std::optional<active_attribute_info> tryGetActiveAttribute(const std::string &aAttributeName) const;

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
        void setUniform(const std::string &aName, const gdk::webgl1es2_texture &aTexture) const;

        //TODO: texture needs to support more than tex2d!
        //! bind an array of textures to the context then assign them to texture uniforms
        //void setUniform(const std::string &aName, const gdk::texture &aTexture) const;

        //! Installs this program's shaders to their corresponding programmable stages 
        /// will be used for subsequent draw calls until a different program is installed.
        GLuint useProgram() const;
                
        /// \brief equality semantics
        bool operator==(const webgl1es2_shader_program &) const; 
        /// \brief equality semantics
        bool operator!=(const webgl1es2_shader_program &) const; 

        /// \brief move semantics
        webgl1es2_shader_program(webgl1es2_shader_program &&) = default;
        /// \brief move semantics
        webgl1es2_shader_program &operator=(webgl1es2_shader_program &&) = default;
        
        /// \brief constructs a shader program with glsl source file contents for a vertex shader and a fragment shader 
        webgl1es2_shader_program(std::string aVertexSource, std::string aFragmentSource);

        //! shader useful for indicating some kind of failure. Performs MVP mul then colors all frags bright pink.
        static const jfc::shared_proxy_ptr<gdk::webgl1es2_shader_program> PinkShaderOfDeath;

        //! shader for drawing unlit surfaces with alpha channel based fragment discard. Suitable for text rendering, 
        /// GUI element rendering, 2D Sprite rendering. Extremely lightweight.
        static const jfc::shared_proxy_ptr<gdk::webgl1es2_shader_program> AlphaCutOff;

        //! 8 is the guaranteed minimum across all es2/web1 implementations. 
        /// Can check against max but that invites the possibility of shaders working on some impls and not others.. want to avoid that, 
        /// therefore define the "max" as the guaranteed minimum.
        static const size_t MAX_TEXTURE_UNITS = 8;
    };
}

#endif
