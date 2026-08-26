// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CONTEXT_H
#define GDK_GFX_CONTEXT_H

#include <gdk/graphics/entity.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/material.h>
#include <gdk/graphics/model.h>
#include <gdk/graphics/scene.h>
#include <gdk/graphics/screen_camera.h>
#include <gdk/graphics/shader_program.h>
#include <gdk/graphics/texture.h>
#include <gdk/graphics/texture_camera.h>

#include <memory>

namespace gdk::graphics {
        //! entry point for gdk-graphics
        class context {
        public:
        /// \name factory methods
        ///@{
        //
            //! makes a scene
            [[nodiscard]] virtual scene_ptr_type make_scene() = 0;

            //! makes a camera 
            [[nodiscard]] virtual camera_ptr_type make_camera() = 0;
            
            //! makes a texture_camera
            [[nodiscard]] virtual texture_camera_ptr_type make_texture_camera() = 0;
           
            //! make an entity
            [[nodiscard]] virtual entity_ptr_type make_entity(
                const const_model_ptr_type pModel, 
                const const_material_ptr_type pMaterial 
            ) = 0;
            
            //! make a model
            [[nodiscard]] virtual model_ptr_type make_model(
                const gdk::graphics::model::usage_hint,
                const model_data &vertexData
            ) = 0;
            //! make an empty model
            [[nodiscard]] virtual model_ptr_type make_model() = 0;

            //! make a material. 
            [[nodiscard]] virtual material_ptr_type make_material(
                const const_shader_ptr_type pShader,
                const material::render_mode aRenderMode = material::render_mode::opaque,
                const material::face_culling_mode aFaceCullingMode = material::face_culling_mode::none
            ) = 0;

            //! make a texture 
            [[nodiscard]] virtual texture_ptr_type make_texture(
                const texture_data::view &aTextureDataView,
                const texture::wrap_mode aWrapModeU = texture::wrap_mode::repeat,
                const texture::wrap_mode aWrapModeV = texture::wrap_mode::repeat
            ) = 0;
            //! make an empty texture 
            [[nodiscard]] virtual texture_ptr_type make_texture() = 0;
        ///@}

        /// \name special resources provided by the implementation
        ///
        /// **Each call builds a new one, and the caller owns what it gets.** These were shared
        /// instances, lazily built once per process and handed to everyone -- which is a gl object
        /// living outside any gl context, and wrong as soon as a second one exists: a handle made in
        /// one context means nothing in another. A caller who wants one cube for a whole program
        /// should make it once and hold it, which is a thing only the caller can decide.
        /// \see ROADMAP_GDK_WINDOW.md §6g
        ///@{
        //
            /// \brief a 1x1x1 cube model
            /// - vertex attributes: vec3 pos, vec2 uv, vec3 normal
            [[nodiscard]] virtual model_ptr_type make_cube_model() const = 0;

            [[nodiscard]] virtual model_ptr_type make_sphere_model() const = 0;

            /// \brief Basic unlit forward renderer program:
            /// Attributes: 
            /// - vec3 pos
            /// - vec2 uv
            /// Uniforms:
            /// - _MVP: model-view-projection matrix 
            /// - _Texture: used to color frags
            /// Vertex shader behavior:
            /// - multiply position by mvp
            /// Fragment shader behavior:
            /// - sample _Texture, if alpha is 0, discard frag, 
            ///   otherwise write frag to color buffer
            [[nodiscard]] virtual shader_ptr_type make_alpha_cutoff_shader() const = 0;
        //@}

            //! virtual destructor
            virtual ~context() = default;
    };
}

#endif

