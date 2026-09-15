// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_SHADER_PROGRAM_H
#define GDK_GFX_WEBGL1ES2_SHADER_PROGRAM_H

#include <gdk/graphics/string_keyed.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/opengl.h>
#include <gdk/graphics/shader_program.h>
#include <gdk/graphics/webgl1es2_texture.h>
#include <jfc/unique_handle.h>
#include <gdk/graphics/webgl1es2_gl_state.h>

#include <jfc/lazy_ptr.h>

#include <memory>

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace gdk::graphics {
    /// \brief Specifies drawing behaviours at the two programmable stages in the OpenGL ES 2.0/WebGL 1.0 pipeline 
    /// (the vertex shader stage and fragment shader stage)
    /// TODO: array uniform methods currently force the use of vectors. should add support for c-style arrays and std::arrays.
    class webgl1es2_shader_program final : public shader_program {
    public:
        /// \brief index, size, type of an active attribute used in the shader program
        struct active_attribute_info {
            GLint location;     //!< location of the attribute
            GLenum type;        //!< type of the attribute's components, e.g: float
            GLint count;        //!< number of attributes
        };

        /// \brief index. size. type o an active uniform used in the shader program
        struct active_uniform_info {
            GLint location;     //!< location of the uniform  e.g: 1
            GLenum type;        //!< type of the uniform e.g: Texture
            GLint size;         //!< size of the uniform e.g: 1
        };

        //! associative collection which maps an active attribute by its name to its 
        /// index in the program. used in memoization strategy to reduce opengl api calls
        using active_attribute_collection_type = string_keyed<active_attribute_info>;
        //! associative collection which maps an active uniform by its name to its 
        /// index in the program. used in memoization strategy to reduce opengl api calls
        using active_uniform_collection_type = string_keyed<active_uniform_info>;

        //! type alias used for setting int2 uniform value
        using integer2_uniform_type = intvector2_type;
        //! type alias used for setting int3 uniform value
        using integer3_uniform_type = intvector3_type;
        //! type alias used for setting int4 uniform value
        using integer4_uniform_type = intvector4_type;

    public:
        //! returns a nonnull optional to an attribute info if one with the given name exists
        std::optional<active_attribute_info> tryGetActiveAttribute(
            const std::string_view aAttributeName) const;

        //! assign a float1 uniform from a float
        /// \brief the location of an active uniform, or -1 if the program does not have one
        ///
        /// A location is fixed for the life of a linked program, so a caller that sets the same
        /// uniform every frame should resolve the name once and use `set_uniform` thereafter,
        /// rather than paying a string hash per frame. \see webgl1es2_material::activate
        [[nodiscard]] GLint uniform_location(const std::string_view aName) const;

        /// \name setting by resolved location
        ///
        ///@{
        void set_uniform(const GLint aLocation, const GLfloat aValue) const;
        void set_uniform(const GLint aLocation, const vector2_type &aValue) const;
        void set_uniform(const GLint aLocation, const vector3_type &aValue) const;
        void set_uniform(const GLint aLocation, const vector4_type &aValue) const;
        void set_uniform(const GLint aLocation, const GLint aValue) const;
        void set_uniform(const GLint aLocation, const integer2_uniform_type &aValue) const;
        void set_uniform(const GLint aLocation, const integer3_uniform_type &aValue) const;
        void set_uniform(const GLint aLocation, const integer4_uniform_type &aValue) const;

        //! an ivec2 array. An empty one names no elements and is skipped
        void set_uniform(const GLint aLocation,
            const std::vector<integer2_uniform_type> &aValue) const;
        ///@}

        bool try_set_uniform(const std::string_view aName, const GLfloat aValue) const;
        //! assign a float2 uniform from a 2 component vector
        bool try_set_uniform(const std::string_view aName, const vector2_type &aValue) const;
        //! assign a float3 uniform from a 3 component vector
        bool try_set_uniform(const std::string_view aName, const vector3_type &aValue) const;
        //! assign a float4 uniform from a 4 component vector
        bool try_set_uniform(const std::string_view aName, const vector4_type &aValue) const;
        //! assign a float uniform array from an array of floats
        bool try_set_uniform(const std::string_view aName, const std::vector<GLfloat> &avalue) const;
        //! assign a float2 uniform array from an array of float2s
        bool try_set_uniform(const std::string_view aName, const std::vector<vector2_type> &avalue) const; 
        //! assign a value to a float3 uniform array from a vector of float3s
        bool try_set_uniform(const std::string_view aName, const std::vector<vector3_type> &avalue) const;
        //! assign an RGBA colour to a float4 uniform
        bool try_set_uniform(const std::string_view aName, const color &aValue) const;

        //! assign a value to a float4 uniform array from a vector of float4s
        bool try_set_uniform(const std::string_view aName, const std::vector<vector4_type> &avalue) const; 

        //! assign a value to a integer uniform
        bool try_set_uniform(const std::string_view aName, const GLint aValue) const;
        //! assign a value to a integer2 uniform
        bool try_set_uniform(const std::string_view aName, const integer2_uniform_type &aValue) const;
        //! assign a value to a integer3 uniform
        bool try_set_uniform(const std::string_view aName, const integer3_uniform_type &aValue) const;
        //! assign a value to a integer4 uniform
        bool try_set_uniform(const std::string_view aName, const integer4_uniform_type &aValue) const;

        //! assign a value to a integer uniform array
        bool try_set_uniform(const std::string_view aName, const std::vector<GLint> &aValue) const;
        //! assign a value to a integer2 uniform array
        bool try_set_uniform(const std::string_view aName, const std::vector<integer2_uniform_type> &aValue) const;
        //! assign a value to a integer3 uniform array
        bool try_set_uniform(const std::string_view aName, const std::vector<integer3_uniform_type> &aValue) const;
        //! assign a value to a integer4 uniform array
        bool try_set_uniform(const std::string_view aName, const std::vector<integer4_uniform_type> &aValue) const;

        //! assign a value to a bool uniform 
        //bool try_set_uniform(const std::string_view aName, const bool &aValue) const;
        //! assign a value to a bvec2 uniform
        //bool try_set_uniform(const std::string_view aName, const boolean2_uniform_type &a) const;
        //! assign a value to a bvec3 uniform
        //bool try_set_uniform(const std::string_view aName, const boolean3_uniform_type &a) const;
        //! assign a value to a bvec4 uniform
        //bool try_set_uniform(const std::string_view aName, const boolean4_uniform_type &a) const;

        //! assign a value to a bool uniform array 
        //bool try_set_uniform(const std::string_view aName, const std::vector<bool> &aValue) const;
        //! assign a value to a bvec2 uniform array
        //bool try_set_uniform(const std::string>&aName, const std::vector<boolean2_uniform_type> &a) const;
        //! assign a value to a bvec3 uniform array
        //bool try_set_uniform(const std::string>&aName, const std::vector<boolean3_uniform_type> &a) const;
        //! assign a value to a bvec4 uniform array
        //bool try_set_uniform(const std::string>&aName, const std::vector<boolean4_uniform_type> &a) const;

        /*//! assign a mat2x2 uniform from a mat2x2
        bool try_set_uniform(const std::string_view aName, const mat2x2_type &avalue) const; 
        //! assign a mat2x2 uniform array from a vector of mat2x2s
        bool try_set_uniform(const std::string_view aName, const std::vector<mat2x2_type> &avalue) const;*/

        //! assign a mat3x3 uniform from a mat3x3
        bool try_set_uniform(const std::string_view aName, const mat3x3_type &avalue) const;
        //! assign a mat3x3 uniform array from a vector of mat3x3s
        bool try_set_uniform(const std::string_view aName,
            const std::vector<mat3x3_type> &avalue) const;

        //! assign a mat4x4 uniform from a mat4x4
        bool try_set_uniform(const std::string_view aName, const matrix4x4_type &avalue) const; 
        //! assign a mat4x4 uniform array from a vector of mat4x4s
        bool try_set_uniform(const std::string_view aName, const std::vector<matrix4x4_type> &avalue) const; 

        //TODO: texture needs to support more than tex2d!
        //! bind a texture to the context then assign it to a texture uniform
        bool try_set_uniform(const std::string_view aName, const gdk::graphics::webgl1es2_texture &aTexture,
            gl_state &aState) const;

        /// \brief set the four standard matrices at once, by cached location
        ///
        /// What an entity calls every frame. Equivalent to four `try_set_uniform` calls naming
        /// `_Model`, `_View`, `_Projection` and `_MVP`, without the four string hashes 
        void set_standard_matrices(const matrix4x4_type &aModel, const matrix4x4_type &aView,
            const matrix4x4_type &aProjection, const matrix4x4_type &aMVP) const;

        //TODO: texture needs to support more than tex2d!
        //! bind an array of textures to the context then assign them to texture uniforms
        //bool try_set_uniform(const std::string_view aName, const gdk::graphics::texture &aTexture) const;

        //! Installs this program's shaders to their corresponding programmable stages 
        /// will be used for subsequent draw calls until a different program is installed.
        void useProgram(gl_state &aState) const;

        /// \brief the gl name of the linked program
        [[nodiscard]] GLuint handle() const;
                
        /// \brief equality semantics
        bool operator==(const webgl1es2_shader_program &) const; 
        /// \brief equality semantics

        /// \brief move semantics
        webgl1es2_shader_program(webgl1es2_shader_program &&) = default;
        /// \brief move semantics
        webgl1es2_shader_program &operator=(webgl1es2_shader_program &&) = default;
        
        /// \brief constructs a shader program with glsl source file contents for a vertex shader and a fragment shader 
        webgl1es2_shader_program(std::string aVertexSource, std::string aFragmentSource);

        /// \brief a shader useful for indicating failure: mvp multiply, then every fragment bright pink
        [[nodiscard]] static std::shared_ptr<webgl1es2_shader_program> make_pink_shader_of_death();

        /// \brief unlit surfaces with alpha-based fragment discard
        ///
        /// Suitable for text, gui elements and 2d sprites; extremely lightweight.
        [[nodiscard]] static std::shared_ptr<webgl1es2_shader_program> make_alpha_cutoff();

 //! \brief keeps the texel's alpha rather than discarding on it, for the blender to use
 static std::shared_ptr<webgl1es2_shader_program> make_alpha_blend();

        static size_t MAX_TEXTURE_UNITS();
        static size_t MAX_FRAGMENT_SHADER_INSTRUCTIONS();
        static size_t MAX_VERTEX_SHADER_INSTRUCTIONS();
    
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

        /// \brief locations of the four matrices every entity sets each frame
        GLint m_ModelLocation = -1;
        GLint m_ViewLocation = -1;
        GLint m_ProjectionLocation = -1;
        GLint m_MVPLocation = -1;
    };
}

#endif

