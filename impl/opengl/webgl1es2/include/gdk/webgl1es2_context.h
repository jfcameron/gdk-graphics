// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_CONTEXT_H
#define GDK_GFX_WEBGL1ES2_CONTEXT_H

#include <gdk/graphics_context.h>

#include <string_view>

namespace gdk {
    class model_data;

    //! brief webgl1/gles2.0 context implementation
    class webgl1es2_context final : public graphics::context {
    public: 
        [[nodiscard]] static graphics_context_ptr_type make();

        [[nodiscard]] graphics_shader_ptr_type make_shader(
            const std::string_view aVertexShaderStageSourceCodeGLSL, 
            const std::string_view aFragmentShaderStageSourceCodeGLSL
        );

        [[nodiscard]] virtual graphics_scene_ptr_type make_scene() override;

        [[nodiscard]] virtual graphics_camera_ptr_type make_camera() override;

        [[nodiscard]] virtual graphics_texture_camera_ptr_type make_texture_camera() override;

        [[nodiscard]] virtual graphics_entity_ptr_type make_entity(
            const const_graphics_model_ptr_type pModel, 
            const const_graphics_material_ptr_type pMaterial
        ) override;

        [[nodiscard]] virtual graphics_material_ptr_type make_material(
            const const_graphics_shader_ptr_type pShader,
            const material::render_mode aRenderMode,
            const material::face_culling_mode aface_culling_mode
        ) override;

        [[nodiscard]] virtual graphics_model_ptr_type make_model() override;

        [[nodiscard]] virtual graphics_model_ptr_type make_model(
            const gdk::model::usage_hint, 
            const model_data &vertexDataView
        ) override;

        [[nodiscard]] virtual graphics_texture_ptr_type make_texture(
            const texture_data::view &imageView,
            const texture::wrap_mode,
            const texture::wrap_mode
        ) override;

        [[nodiscard]] virtual graphics_texture_ptr_type make_texture() override;

        [[nodiscard]] virtual graphics_shader_ptr_type get_alpha_cutoff_shader() const override;

        [[nodiscard]] virtual graphics_model_ptr_type get_cube_model() const override;
        
        webgl1es2_context(webgl1es2_context &&);

        virtual ~webgl1es2_context() override;

    private:
        webgl1es2_context(); 
    };
}

#endif

