// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CONTEXT_H
#define GDK_GFX_CONTEXT_H

#include <gdk/entity.h>
#include <gdk/graphics_types.h>
#include <gdk/material.h>
#include <gdk/model.h>
#include <gdk/scene.h>
#include <gdk/screen_camera.h>
#include <gdk/shader_program.h>
#include <gdk/texture.h>
#include <gdk/texture_camera.h>

#include <memory>

namespace gdk {
    namespace graphics {
        //! entry point for gdk-graphics
        class context {
        public:
        /// \name factory methods
        ///@{
        //
            //! makes a scene
            [[nodiscard]] virtual graphics_scene_ptr_type make_scene() = 0;

            //! makes a camera 
            [[nodiscard]] virtual graphics_camera_ptr_type make_camera() = 0;
            
            //! makes a texture_camera
            [[nodiscard]] virtual graphics_texture_camera_ptr_type make_texture_camera() = 0;
           
            //! make an entity
            [[nodiscard]] virtual graphics_entity_ptr_type make_entity(
                const const_graphics_model_ptr_type pModel, 
                const const_graphics_material_ptr_type pMaterial 
            ) = 0;
            
            //! make a model
            [[nodiscard]] virtual graphics_model_ptr_type make_model(
                const gdk::model::usage_hint,
                const model_data &vertexData
            ) = 0;
            //! make an empty model
            [[nodiscard]] virtual graphics_model_ptr_type make_model() = 0;

            //! make a material. 
            [[nodiscard]] virtual graphics_material_ptr_type make_material(
                const const_graphics_shader_ptr_type pShader,
                const material::render_mode aRenderMode = material::render_mode::opaque,
                const material::face_culling_mode aFaceCullingMode = material::face_culling_mode::none
            ) = 0;

            //! make a texture 
            [[nodiscard]] virtual graphics_texture_ptr_type make_texture(
                const texture_data::view &aTextureDataView,
                const texture::wrap_mode aWrapModeU = texture::wrap_mode::repeat,
                const texture::wrap_mode aWrapModeV = texture::wrap_mode::repeat
            ) = 0;
            //! make an empty texture 
            [[nodiscard]] virtual graphics_texture_ptr_type make_texture() = 0;
        ///@}

        /// \name special resources provided by the implementation
        ///@{
        //
            /// \brief a 1x1x1 cube model
            /// - vertex attributes: vec3 pos, vec2 uv, vec3 normal
            [[nodiscard]] virtual graphics_model_ptr_type get_cube_model() const = 0;

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
            [[nodiscard]] virtual graphics_shader_ptr_type get_alpha_cutoff_shader() const = 0;
        //@}

            //! virtual destructor
            virtual ~context() = default;
        };
    }
}

#endif

