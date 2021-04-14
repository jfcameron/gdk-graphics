// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_MATERIAL_H
#define GDK_GFX_WEBGL1ES2_MATERIAL_H

#include <gdk/material.h>
#include <gdk/webgl1es2_shader_program.h>

#include <unordered_map>
#include <array>
#include <optional>

namespace gdk
{
    /// \todo think about how to model blend functionality. 
    /// subclass that provides blend func related state , in opaque case force opaque options?
    class webgl1es2_material : public material
    {
    public:
    /// \name external interface
    ///@{
    //
        virtual void setTexture(const std::string& aName, texture_ptr_type aTexture) override;
        
        virtual void setFloat(const std::string& aName, float aValue) override;
        virtual void setVector2(const std::string& aName, graphics_vector2_type aValue) override;
        virtual void setVector3(const std::string &aName, graphics_vector3_type aValue) override;
        virtual void setVector4(const std::string &aName, graphics_vector4_type aValue) override;

        virtual void setInteger(const std::string& aName, int aValue) override;
        virtual void setInteger2(const std::string& aName, int aValue1, int aValue2) override;
        virtual void setInteger3(const std::string& aName, int aValue1, int aValue2, 
            int aValue3) override;
        virtual void setInteger4(const std::string& aName, int aValue1, int aValue2, 
            int aValue3, int aValue4) override;
    ///@}
        
        //! shaders can be shared among many webgl1es2_materials
        using shader_ptr_type = std::shared_ptr<gdk::webgl1es2_shader_program>;

    /// \name internal interface
    ///@{
    //
        material::render_mode get_render_mode() const;

        shader_ptr_type getShaderProgram();
        //! modifies the opengl state, assigning the program, assigning values to the program's uniforms etc.
        void activate();
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
            material::FaceCullingMode aFaceCullingMode,
            material::render_mode aRenderMode);

        //! trivial destructor
        ~webgl1es2_material() = default;

    private:
        using texture_ptr_impl_type = std::shared_ptr<webgl1es2_texture>;

        //! associative collection: uniform name to texture data
        using texture_uniform_collection_type = 
            std::unordered_map<std::string, texture_ptr_impl_type>;

        //! the shader used by the webgl1es2_material
        shader_ptr_type m_pShaderProgram;
        
        //! Whether or not to discard polygons based on [entity space] normal direction
        material::FaceCullingMode m_FaceCullMode = material::FaceCullingMode::None;

        //! render mode or opaque
        material::render_mode m_RenderMode = material::render_mode::opaque;

        //! texture data provided to the shader stages
        texture_uniform_collection_type m_Textures;
        
        //! float data provided to the shader stages
        std::unordered_map<std::string, float> m_Floats;

        //! vector2 data provided to the shader stages
        std::unordered_map<std::string, graphics_vector2_type> m_Vector2s;

        //! vector3 data provided to the shader stages
        std::unordered_map<std::string, graphics_vector3_type> m_Vector3s;
        
        //! vector4 data provided to the shader stages
        std::unordered_map<std::string, graphics_vector4_type> m_Vector4s;

        //! integer data provided to the shader stages
        std::unordered_map<std::string, int> m_Integers;
        
        //! integer data provided to the shader stages
        std::unordered_map<std::string, std::array<int, 2>> m_Integer2s;

        //! integer data provided to the shader stages
        std::unordered_map<std::string, std::array<int, 3>> m_Integer3s;
        
        //! integer data provided to the shader stages
        std::unordered_map<std::string, std::array<int, 4>> m_Integer4s;
    };
}

#endif

