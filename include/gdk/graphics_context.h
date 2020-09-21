// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CONTEXT_H
#define GDK_GFX_CONTEXT_H

#include <memory>

#include <gdk/scene.h>
#include <gdk/camera.h>
#include <gdk/entity.h>
#include <gdk/material.h>
#include <gdk/model.h>
#include <gdk/shader_program.h>
#include <gdk/texture.h>
#include <gdk/vertex_data_view.h>

namespace gdk::graphics
{
    //! entry point for gdk-graphics
    /// to use, call make with the desired implementation
    /// implementation chooses what Graphics API/API Standard to use
    /// \warn the context is not responsible for establishing a context on the graphical hardware,
    /// \warn The context does not know about the windowing system (or lack thereof)
    /// \warn Initializing OpenGL etc. to the correct standard, managing & swapping buffers etc., creating and managing X11 windows, etc. must be done independently
    class context
    {
    public:
        //! ptr type returned by factory method
        using context_ptr_type = std::unique_ptr<context>;
        //! scene factory return type
        using scene_ptr_type = std::unique_ptr<scene>;
        //! camera factory return type
        using camera_ptr_type = std::unique_ptr<camera>;
        //! entity factory return type
        using entity_ptr_type = std::unique_ptr<entity>;
        //! shader_program factory return type
        using shader_program_ptr_type = std::unique_ptr<shader_program>;
        //! model factory return type
        using model_ptr_type = std::unique_ptr<model>;
        //! material factory return type
        using material_ptr_type = std::unique_ptr<material>;
        //! texture factory return type
        using texture_ptr_type = std::unique_ptr<texture>;
        
        //! ptr type for built in models provided by the implementation
        using built_in_model_ptr_type = std::shared_ptr<model>;
        //! ptr type for built in shaders provided by the implementation
        using built_in_shader_ptr_type = std::shared_ptr<shader_program>;
        
        //! shared ptr to a shader_program
        using shader_program_shared_ptr_type = std::shared_ptr<shader_program>;
        //! shared ptr to a material
        using material_shared_ptr_type = std::shared_ptr<material>;
        //! shared ptr to a model
        using model_shared_ptr_type = std::shared_ptr<model>;

        //! specifies implementation to use in context construction
        enum class implementation
        {
            //! limited subset of OpenGL API available on a wide variety of desktop, 
            /// mobile devices and browsers
            opengl_webgl1_gles2,
        };

        //! context factory method
        static context_ptr_type make(const implementation &);

        //! makes a scene
        virtual scene_ptr_type make_scene() const = 0;

        //! makes a camera
        virtual camera_ptr_type make_camera() const = 0;
        //! makes a camera by copy
        virtual camera_ptr_type make_camera(
            const camera &other //!< camera to copy
        ) const = 0;
       
        //! make an entity
        virtual entity_ptr_type make_entity(
            model_shared_ptr_type pModel, //!< vertex data for this entity. describes modelspace data.
            material_shared_ptr_type pMaterial //!<  decides how the vertex data is ultimately rendered through the pipeline.
        ) const = 0;
        //! make an entity by copy
        virtual entity_ptr_type make_entity(
            const entity &other //!< entity to copy
        ) const = 0;
       
        //! construct model by vertext data view
        virtual model_ptr_type make_model(const vertex_data_view &vertexDataView) const = 0;

        //! make a material. 
        virtual material_ptr_type make_material(
            shader_program_shared_ptr_type pShader //!< defines the pipeline's programmable stage behaviours, can be shared among multiple materials
        ) const = 0;

        /// \brief make a shader program containing a user-defined vertex shader stage and fragment shader stage
		/// TODO: consider returning a shared pointer. The reality is shaders are used by materials and expected to be shared. unique doesnt really add value here.
        virtual shader_program_ptr_type make_shader(const std::string &aVertexStageSource, const std::string &aFragmentStageSource) const = 0;

        //! make a texture using a 2d image view
        virtual texture_ptr_type make_texture(const texture::image_data_2d_view &imageView) const = 0;

		//! make a texture from an in-memory RGBA32 PNG
		virtual texture_ptr_type make_texture(const std::vector<std::underlying_type<std::byte>::type> &aRGBA32PNGData) const = 0;

        /**
         * @name special resources provided by the implementation, focused on being resource unintensive.
         */
        //@{
        /// \brief A forward renderer shader program with the following properties:
        /// - required attributes: vec3 pos, vec2 uv
        /// - vertex shader: mvp mul only
        /// - fragment shader: colors frags with 2d sampler named "_Texture"
        /// - If alpha channel is < 1.0, the fragment is discarded
        virtual built_in_shader_ptr_type get_alpha_cutoff_shader() const = 0;

        /// \brief A forward renderer shader program useful for displaying render errors:
        /// - required attributes: vec3 pos
        /// - vertex shader: mvp mul
        /// - fragment shader: all frags are colored neon pink
        virtual built_in_shader_ptr_type get_pink_shader_of_death() const = 0;

        /// \brief a 1x1x1 cube model
        /// - vertex attributes: vec3 pos, vec2 uv, vec3 normal
        virtual built_in_model_ptr_type get_cube_model() const = 0;

        /// \brief a 1x1 quad model
        /// - vertex attributes: vec3 pos, vec2 uv, vec3 normal
        virtual built_in_model_ptr_type get_quad_model() const = 0;
        //@}

        //! virtual destructor
        virtual ~context() = default;

    protected:
        //! prevent instantiation; context is the interface to all context implementations
        context() = default;
    };
}

#endif
