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


        [[nodiscard]] virtual shader_program_ptr_type make_shader(
            const std::string &aVertexGLSL, 
            const std::string &aFragGLSL
        ) const override;

        [[nodiscard]] virtual graphics::context::texture_ptr_type make_texture(
            const image_data_2d_view &imageView
        ) const override;

        [[nodiscard]] virtual graphics::context::built_in_shader_ptr_type get_alpha_cutoff_shader() const override;

        [[nodiscard]] virtual built_in_shader_ptr_type get_pink_shader_of_death() const override;

        [[nodiscard]] virtual built_in_model_ptr_type get_cube_model() const override;
        
        [[nodiscard]] virtual built_in_model_ptr_type get_quad_model() const override;

        virtual ~webgl1es2_context() override = default;

        //! context factory method
        [[nodiscard]] static context_ptr_type make();

    private:
        webgl1es2_context(); 
    };
}

#endif
