// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_CONTEXT_H
#define GDK_GFX_WEBGL1ES2_CONTEXT_H

#include <gdk/graphics/context.h>
#include <gdk/graphics/webgl1es2_gl_state.h>

#include <memory>

#include <string_view>

namespace gdk::graphics {
    class model_data;

    //! brief webgl1/gles2.0 context implementation
    class webgl1es2_context final : public graphics::context {
    public: 
        [[nodiscard]] static context_ptr_type make();

        [[nodiscard]] shader_ptr_type make_shader(
            const std::string_view aVertexShaderStageSourceCodeGLSL, 
            const std::string_view aFragmentShaderStageSourceCodeGLSL
        );

        [[nodiscard]] virtual scene_ptr_type make_scene() override;

        [[nodiscard]] virtual camera_ptr_type make_camera() override;

        [[nodiscard]] virtual texture_camera_ptr_type make_texture_camera() override;

        [[nodiscard]] virtual entity_ptr_type make_entity(
            const const_model_ptr_type pModel, 
            const const_material_ptr_type pMaterial
        ) override;

        [[nodiscard]] virtual material_ptr_type make_material(
            const const_shader_ptr_type pShader,
            const material::render_mode aRenderMode,
            const material::face_culling_mode aface_culling_mode
        ) override;

        [[nodiscard]] virtual material_ptr_type make_material(
            const const_material_ptr_type &aPrototype) override;

        [[nodiscard]] virtual model_ptr_type make_model() override;

        [[nodiscard]] virtual model_ptr_type make_model(
            const gdk::graphics::model::usage_hint, 
            const model_data &vertexDataView
        ) override;

        [[nodiscard]] virtual texture_ptr_type make_texture(
            const texture_data::view &imageView,
            const texture::wrap_mode,
            const texture::wrap_mode,
            const texture::filter_mode = texture::filter_mode::sharp
        ) override;

        [[nodiscard]] virtual texture_ptr_type make_texture() override;

        [[nodiscard]] virtual shader_ptr_type make_alpha_cutoff_shader() const override;

        [[nodiscard]] virtual shader_ptr_type make_alpha_blend_shader() const override;

        [[nodiscard]] virtual size_t max_texture_size() const override;

        [[nodiscard]] virtual model_ptr_type make_cube_model() const override;

        [[nodiscard]] virtual model_ptr_type make_sphere_model() const override;
        
        webgl1es2_context(webgl1es2_context &&);

        virtual ~webgl1es2_context() override;

        /// \brief what this gl context currently has set
        [[nodiscard]] std::shared_ptr<gl_state> state() const;

    private:
        webgl1es2_context();

        std::shared_ptr<gl_state> m_pState;

        size_t m_MaxTextureSize = 0;
    };
}

#endif

