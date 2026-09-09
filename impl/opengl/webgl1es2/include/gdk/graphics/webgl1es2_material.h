// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_MATERIAL_H
#define GDK_GFX_WEBGL1ES2_MATERIAL_H

#include <gdk/graphics/webgl1es2_gl_state.h>
#include <gdk/graphics/string_keyed.h>
#include <gdk/graphics/material.h>
#include <gdk/graphics/webgl1es2_shader_program.h>

#include <array>
#include <optional>
#include <unordered_map>

namespace gdk::graphics {
    /// \todo think about how to model blend functionality. 
    /// subclass that provides blend func related state , in opaque case force opaque options?
    class webgl1es2_material : public material {
    public:
    /// \name external interface
    ///@{
    //
        virtual void set_texture(const std::string_view aName, const texture_ptr_type aTexture) override;
        
        virtual void set_float(const std::string_view aName, float aValue) override;
        virtual void set_vector2(const std::string_view aName, vector2_type aValue) override;
        virtual void set_vector3(const std::string_view aName, vector3_type aValue) override;
        virtual void set_vector4(const std::string_view aName, vector4_type aValue) override;
        virtual void set_vector4(const std::string_view aName, const color &aValue) override;

        virtual void set_integer(const std::string_view aName, int aValue) override;
        virtual void set_integer2(const std::string_view aName, int aValue1, int aValue2) override;
        virtual void set_integer3(const std::string_view aName, int aValue1, int aValue2, int aValue3) override;
        virtual void set_integer4(const std::string_view aName, int aValue1, int aValue2, int aValue3, int aValue4) override;

        virtual void set_int_vector2_array(const std::string_view aName, const std::vector<intvector2_type> &aValue) override;
    ///@}
        
        //! shaders can be shared among many webgl1es2_materials
        using shader_ptr_type = std::shared_ptr<gdk::graphics::webgl1es2_shader_program>;

    /// \name internal interface
    ///@{
    //
        material::render_mode get_render_mode() const;

        shader_ptr_type getShaderProgram();
        //! modifies the opengl state, assigning the program, assigning values to the program's uniforms etc.
        //! \param aState the gl context's cache. \see gl_state
        void activate(gl_state &aState);
    ///@}
        //! support move semantics
        webgl1es2_material &operator=(webgl1es2_material&&) = default;
        //! support move semantics
        webgl1es2_material(webgl1es2_material&&) = default;

        //! support copy semantics
        webgl1es2_material &operator=(const webgl1es2_material&) = default;
        //! support copy semantics
        webgl1es2_material(const webgl1es2_material&) = default;

        //! constructs a webgl1es2_material.
        /// non-shader pipeline state and uniform values are 0 constructed, 
        /// given default values specified by the opengl standard
        /// however a shader must be provided at ctor time, 
        /// since the pipeline cannot be invoked without first using a shader
        /// logically, a webgl1es2_material without a shader_program implies 
        /// a pipeline without a programmable vertex shader stage nor a 
        /// programmable fragment shader stage, which is not a valid pipeline
        webgl1es2_material(shader_ptr_type pShader,
            material::face_culling_mode aface_culling_mode,
            material::render_mode aRenderMode);

        //! trivial destructor
        ~webgl1es2_material() = default;

    private:
        using texture_ptr_impl_type = std::shared_ptr<webgl1es2_texture>;

        //! associative collection: uniform name to texture data
        using texture_uniform_collection_type = 
            string_keyed<texture_ptr_impl_type>;

        //! the shader used by the webgl1es2_material
        shader_ptr_type m_pShaderProgram;
        
        //! Whether or not to discard polygons based on [entity space] normal direction
        material::face_culling_mode m_FaceCullMode = material::face_culling_mode::none;

        //! render mode or opaque
        material::render_mode m_RenderMode = material::render_mode::opaque;

        //! texture data provided to the shader stages
        texture_uniform_collection_type m_Textures;
        
        /// \brief a value bound for the shader stages, with its location already resolved
        template<typename value_type>
        struct uniform final {
            GLint location{-1};
            value_type value{};
        };

        /// \brief store a value and resolve its location if this is the first time it is set
        template<typename value_type, typename collection_type>
        void assign(collection_type &aCollection, const std::string_view aName,
            const value_type &aValue) {
            if (const auto found = aCollection.find(aName); found != aCollection.end()) {
                found->second.value = aValue;

                return;
            }

            const auto entry = aCollection.emplace(aName, typename collection_type::mapped_type{})
                .first;

            entry->second.location = m_pShaderProgram->uniform_location(entry->first);
            entry->second.value = aValue;
        }

        string_keyed<uniform<float>> m_Floats;
        string_keyed<uniform<vector2_type>> m_Vector2s;
        string_keyed<uniform<vector3_type>> m_vector3s;
        string_keyed<uniform<vector4_type>> m_vector4s;
        string_keyed<uniform<int>> m_Integers;
        string_keyed<uniform<intvector2_type>> m_Integer2s;
        string_keyed<uniform<intvector3_type>> m_Integer3s;
        string_keyed<uniform<intvector4_type>> m_Integer4s;
        string_keyed<uniform<std::vector<intvector2_type>>> m_IntVector2Arrays;
    };
}

#endif

