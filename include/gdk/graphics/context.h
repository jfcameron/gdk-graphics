// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_CONTEXT_H
#define GDK_GRAPHICS_CONTEXT_H

#include <gdk/graphics/entity.h>
#include <gdk/graphics/limits.h>
#include <gdk/graphics/types.h>

#include <cstddef>
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

            //! makes a camera that draws to the screen
            [[nodiscard]] virtual screen_camera_ptr_type make_camera() = 0;
            
            //! makes a texture_camera drawing into textures 1024 texels square
            [[nodiscard]] virtual texture_camera_ptr_type make_texture_camera() = 0;

            /// \brief makes a texture_camera drawing into textures aSize texels across
            [[nodiscard]] virtual texture_camera_ptr_type make_texture_camera(const intvector2_type &aSize) = 0;
           
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
                const material::render_mode aRenderMode,
                const material::face_culling_mode aFaceCullingMode
            ) = 0;

            //! make a material that culls no faces
            [[nodiscard]] material_ptr_type make_material(const const_shader_ptr_type pShader,
                const material::render_mode aRenderMode) {
                return make_material(pShader, aRenderMode, material::face_culling_mode::none);
            }

            //! make an opaque material that culls no faces
            [[nodiscard]] material_ptr_type make_material(const const_shader_ptr_type pShader) {
                return make_material(pShader, material::render_mode::opaque, material::face_culling_mode::none);
            }

            /// \brief make a material that starts as a copy of another
            [[nodiscard]] virtual material_ptr_type make_material(
                const const_material_ptr_type &aPrototype) = 0;

            //! make a texture
            [[nodiscard]] virtual texture_ptr_type make_texture(
                const texture_data::view &aTextureDataView,
                const texture::wrap_mode aWrapModeU,
                const texture::wrap_mode aWrapModeV,
                const texture::filter_mode aFilterMode
            ) = 0;

            //! make a sharp texture
            [[nodiscard]] texture_ptr_type make_texture(const texture_data::view &aTextureDataView,
                const texture::wrap_mode aWrapModeU, const texture::wrap_mode aWrapModeV) {
                return make_texture(aTextureDataView, aWrapModeU, aWrapModeV, texture::filter_mode::sharp);
            }

            //! make a sharp texture that repeats
            [[nodiscard]] texture_ptr_type make_texture(const texture_data::view &aTextureDataView) {
                return make_texture(aTextureDataView, texture::wrap_mode::repeat, texture::wrap_mode::repeat,
                    texture::filter_mode::sharp);
            }

            //! make an empty texture 
            [[nodiscard]] virtual texture_ptr_type make_texture() = 0;
        ///@}

        /// \name special resources provided by the implementation
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
            ///
            /// Both built-in shaders multiply the texel by `_Color`, a vec4 the material may set:
            /// a tint, a fade, a dim. A material starts with it white, so nothing changes until
            /// it is set.
            [[nodiscard]] virtual shader_ptr_type make_alpha_cutoff_shader() const = 0;

            /// \brief a shader that keeps the texel's alpha, for the blender to use.
            ///
            /// The sibling of the alpha cutoff shader: the two differ only in what the fragment does
            /// with alpha, one discarding anything not fully opaque and this one passing it through.
            /// Pair it with material::render_mode::transparent, which is what enables blending.
            [[nodiscard]] virtual shader_ptr_type make_alpha_blend_shader() const = 0;

            /// \brief the limits of the device, narrowed to the context's profile when it was given one
            ///
            /// What an implementation offers beyond this interface, shaders made from source in its own language
            /// say, is on its own context type, which its `make` returns.
            [[nodiscard]] virtual graphics::limits limits() const = 0;
        //@}

            //! virtual destructor
            virtual ~context() = default;
    };
}

#endif

