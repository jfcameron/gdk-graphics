// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_MATERIAL_H
#define GDK_GFX_WEBGL1ES2_MATERIAL_H

#include <gdk/material.h>
#include <gdk/webgl1es2_shader_program.h>

#include <unordered_map>

namespace gdk
{
    /// \todo think about how to model blend functionality. -> subclass that provides blend func related state , in opaque case force opaque options?
    class webgl1es2_material : public material
    {
    public:
    /// \name external interface
    ///@{
    //
        virtual void setFloat(const std::string& aTextureName, float aTexture) override;

        virtual void setTexture(const std::string& aTextureName, texture_ptr_type aTexture) override;

        virtual void setVector2(const std::string& aVector2Name, graphics_vector2_type aVector2) override;
    ///@}

    /// \name internal interface
    ///@{
    //
        //! shaders can be shared among many webgl1es2_materials
        using shader_ptr_type = std::shared_ptr<gdk::webgl1es2_shader_program>;

        shader_ptr_type getShaderProgram();

        //! modifies the opengl state, assigning the program, assigning values to the program's uniforms etc.
        void activate();

        //! support move semantics
        webgl1es2_material& operator=(webgl1es2_material&&) = default;
        //! support move semantics
        webgl1es2_material(webgl1es2_material&&) = default;

        //! support copy semantics
        webgl1es2_material& operator=(const webgl1es2_material&) = default;
        //! support copy semantics
        webgl1es2_material(const webgl1es2_material&) = default;

        //! constructs a webgl1es2_material.
        /// non-shader pipeline state and uniform values are 0 constructed, given default values specified by the opengl standard
        /// however a shader must be provided at ctor time, since the pipeline cannot be invoked without first using a shader
        /// logically, a webgl1es2_material without a shader_program implies a pipeline without a programmable vertex shader stage 
        /// nor a programmable fragment shader stage, which is not a valid pipeline
        webgl1es2_material(shader_ptr_type pShader);

        //! trivial destructor
        ~webgl1es2_material() = default;
    ///@}

    private:
        using texture_ptr_impl_type = std::shared_ptr<webgl1es2_texture>;

        //! associative collection: uniform name to texture data
        using texture_uniform_collection_type = std::unordered_map<std::string, texture_ptr_impl_type>;

        //! uniform name to vector2 data
        using vector2_uniform_collection_type = std::unordered_map<std::string, graphics_vector2_type>;

        //! uniform name to float data
        using float_uniform_collection_type = std::unordered_map<std::string, float>;

        //! the shader used by the webgl1es2_material
        shader_ptr_type m_pShaderProgram;

        //! texture data provided to the shader stages
        texture_uniform_collection_type m_Textures;

		//! vector2 data provided to the shader stages
		vector2_uniform_collection_type m_Vector2s;

		//! float data provided to the shader stages
		float_uniform_collection_type m_Floats;

    public:
    
    };
}

#endif
