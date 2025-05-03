// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_CONTEXT_H
#define GDK_GFX_WEBGL1ES2_CONTEXT_H

#include <gdk/graphics_context.h>

namespace gdk
{
    //! brief webgl1/gles2.0 context implementation
    class webgl1es2_context final : public graphics::context
    {
    public: 
        /// \brief context factory method
        [[nodiscard]] static context_ptr_type make();

        /// \brief make a shader program containing a user-defined vertex shader stage and fragment shader stage
        [[nodiscard]] virtual shader_program_ptr_type make_shader(
            const std::string &aVertexShaderStageSourceCodeGLSL, 
            const std::string &aFragmentShaderStageSourceCodeGLSL
        ) const;

        [[nodiscard]] virtual scene_ptr_type make_scene() const override;

        [[nodiscard]] virtual std::shared_ptr<screen_camera> make_camera() const override;

        [[nodiscard]] virtual std::shared_ptr<screen_camera> make_camera(
            const camera &other
        ) const override;

        [[nodiscard]] virtual std::shared_ptr<texture_camera> make_texture_camera() const override;

        [[nodiscard]] virtual entity_ptr_type make_entity(
            model_shared_ptr_type pModel, 
            material_shared_ptr_type pMaterial
        ) const override;

        [[nodiscard]] virtual entity_ptr_type make_entity(
            const entity& other
        ) const override;

        [[nodiscard]] virtual graphics::context::material_ptr_type make_material(
            shader_program_shared_ptr_type pShader,
            material::render_mode aRenderMode,
            material::FaceCullingMode aFaceCullingMode
        ) const override;

        [[nodiscard]] virtual graphics::context::model_ptr_type make_model() const override;

        [[nodiscard]] virtual graphics::context::model_ptr_type make_model(
            const gdk::model::UsageHint &, 
            const vertex_data &vertexDataView
        ) const override;

        [[nodiscard]] virtual graphics::context::texture_ptr_type make_texture(
            const image_data_2d_view &imageView
        ) const override;

        [[nodiscard]] virtual graphics::context::texture_ptr_type make_texture() const override;

        [[nodiscard]] virtual graphics::context::built_in_shader_ptr_type get_alpha_cutoff_shader() const override;

        [[nodiscard]] virtual built_in_shader_ptr_type get_pink_shader_of_death() const override;

        [[nodiscard]] virtual built_in_model_ptr_type get_cube_model() const override;
        
        [[nodiscard]] virtual built_in_model_ptr_type get_quad_model() const override;

        virtual ~webgl1es2_context() override = default;

    private:
        webgl1es2_context(); 
    };
}

#endif
