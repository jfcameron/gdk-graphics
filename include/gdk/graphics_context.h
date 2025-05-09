// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CONTEXT_H
#define GDK_GFX_CONTEXT_H

#include <gdk/entity.h>
#include <gdk/material.h>
#include <gdk/model.h>
#include <gdk/scene.h>
#include <gdk/shader_program.h>
#include <gdk/texture.h>
#include <gdk/screen_camera.h>
#include <gdk/texture_camera.h>

#include <memory>

namespace gdk::graphics {
    //! entry point for gdk-graphics
    class context {
    public:
        using context_ptr_type = std::shared_ptr<context>;
        using entity_ptr_type = std::shared_ptr<entity>;
        using material_ptr_type = std::shared_ptr<material>;
        using model_ptr_type = std::shared_ptr<model>;
        using scene_ptr_type = std::shared_ptr<scene>;
        using shader_ptr_type = std::shared_ptr<shader_program>;
        using texture_ptr_type = std::shared_ptr<texture>;

    /// \name factory methods
    ///@{
    //
        //! makes a scene
        [[nodiscard]] virtual scene_ptr_type make_scene() const = 0;

        //! makes a camera 
        [[nodiscard]] virtual std::shared_ptr<screen_camera> make_camera() const = 0;
        
        //! makes a texture_camera
        [[nodiscard]] virtual std::shared_ptr<texture_camera> make_texture_camera() const = 0;
       
        //! make an entity
        [[nodiscard]] virtual entity_ptr_type make_entity(
            model_ptr_type pModel, 
            material_ptr_type pMaterial 
        ) const = 0;
        
        //! make a model
        [[nodiscard]] virtual model_ptr_type make_model(
            const gdk::model::usage_hint,
            const vertex_data &vertexData
        ) const = 0;
        //! make an empty model
        [[nodiscard]] virtual model_ptr_type make_model() const = 0;

        //! make a material. 
        [[nodiscard]] virtual material_ptr_type make_material(
            shader_ptr_type pShader,
            material::render_mode aRenderMode = material::render_mode::opaque,
            material::FaceCullingMode aFaceCullingMode = material::FaceCullingMode::None
        ) const = 0;

        //! make a texture 
        [[nodiscard]] virtual texture_ptr_type make_texture(
            const texture_data::view &aTextureDataView,
            const texture::wrap_mode aWrapModeU = texture::wrap_mode::repeat,
            const texture::wrap_mode aWrapModeV = texture::wrap_mode::repeat
        ) const = 0;
        //! make an empty texture 
        [[nodiscard]] virtual texture_ptr_type make_texture() const = 0;
    ///@}

    /// \name special resources provided by the implementation
    ///@{
    //
        /// \brief a 1x1x1 cube model
        /// - vertex attributes: vec3 pos, vec2 uv, vec3 normal
        [[nodiscard]] virtual model_ptr_type get_cube_model() const = 0;

        /// \brief a 1x1 quad model
        /// - vertex attributes: vec3 pos, vec2 uv, vec3 normal
        [[nodiscard]] virtual model_ptr_type get_quad_model() const = 0;

        /// \brief A forward renderer shader program with the following properties:
        /// - required attributes: vec3 pos, vec2 uv
        /// - vertex shader: mvp mul only
        /// - fragment shader: colors frags with 2d sampler named "_Texture"
        /// - If alpha channel is < 1.0, the fragment is discarded
        [[nodiscard]] virtual shader_ptr_type get_alpha_cutoff_shader() const = 0;

        /// \brief A forward renderer shader program useful for displaying render errors:
        /// - required attributes: vec3 pos
        /// - vertex shader: mvp mul
        /// - fragment shader: all frags are colored neon pink
        //TODO: remove this method, init psof at context construction time IF the failure policy is provide a fallback rather than throw
        [[nodiscard]] virtual shader_ptr_type get_pink_shader_of_death() const = 0;

        //TODO: remove this method, init cbod at context construction time IF the failure policy is provide a fallback rather than throw
        //[[nodiscard]] virtual texture_ptr_type get_checkerboard_of_death
    //@}

        //! virtual destructor
        virtual ~context() = default;
    };
}

#endif

