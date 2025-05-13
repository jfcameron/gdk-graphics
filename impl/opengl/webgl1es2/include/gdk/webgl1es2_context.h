// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_CONTEXT_H
#define GDK_GFX_WEBGL1ES2_CONTEXT_H

#include <gdk/graphics_context.h>

namespace gdk
{
    class vertex_data;

    //! brief webgl1/gles2.0 context implementation
    class webgl1es2_context final : public graphics::context
    {
    public: 
        [[nodiscard]] static context_ptr_type make();

        [[nodiscard]] virtual shader_ptr_type make_shader(
            const std::string &aVertexShaderStageSourceCodeGLSL, 
            const std::string &aFragmentShaderStageSourceCodeGLSL
        ) const;

        [[nodiscard]] virtual scene_ptr_type make_scene() const override;

        [[nodiscard]] virtual std::shared_ptr<screen_camera> make_camera() const override;

        [[nodiscard]] virtual std::shared_ptr<texture_camera> make_texture_camera() const override;

        [[nodiscard]] virtual entity_ptr_type make_entity(
            model_ptr_type pModel, 
            material_ptr_type pMaterial
        ) const override;

        [[nodiscard]] virtual material_ptr_type make_material(
            shader_ptr_type pShader,
            material::render_mode aRenderMode,
            material::face_culling_mode aface_culling_mode
        ) const override;

        [[nodiscard]] virtual model_ptr_type make_model() const override;

        [[nodiscard]] virtual model_ptr_type make_model(
            const gdk::model::usage_hint, 
            const vertex_data &vertexDataView
        ) const override;

        [[nodiscard]] virtual texture_ptr_type make_texture(
            const texture_data::view &imageView,
            const texture::wrap_mode,
            const texture::wrap_mode
        ) const override;

        [[nodiscard]] virtual texture_ptr_type make_texture() const override;

        [[nodiscard]] virtual shader_ptr_type get_alpha_cutoff_shader() const override;

        [[nodiscard]] virtual model_ptr_type get_cube_model() const override;
        
        [[nodiscard]] virtual model_ptr_type get_quad_model() const override;

        virtual ~webgl1es2_context() override;

        webgl1es2_context(webgl1es2_context &&);
    private:
        webgl1es2_context(); 
    };
}

#endif

