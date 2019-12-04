// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CONTEXT_H
#define GDK_GFX_CONTEXT_H

#include <memory>

#include <gdk/camera.h>
#include <gdk/entity.h>
#include <gdk/material.h>
#include <gdk/model.h>
#include <gdk/shader_program.h>

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

        //! specifies implementation to use in context construction
        enum class implementation
        {
            //! limited subset of OpenGL API available on a wide variety of desktop, 
            /// mobile devices and browsers
            opengl_webgl1_gles2,
        };

        //! context factory method
        static context_ptr_type make(const implementation &);

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

        //! ptr type for built in models provided by the implementation
        using built_in_model_ptr_type = std::shared_ptr<model>;
        //! ptr type for built in shaders provided by the implementation
        using built_in_shader_ptr_type = std::shared_ptr<shader_program>;

        //! makes a camera
        virtual camera_ptr_type make_camera() const = 0;
        //virtual entity_ptr_type make_entity() const = 0;
        //virtual material_ptr_type make_material(pShader?) const = 0;
        //virtual model_ptr_type make_model(bytes..) const = 0;
        //virtual shader_program_ptr_type make_shader(string aVertexGLSL, string aFragGLSL) = const 0;
        
        //! create a special implementation-provided shader program -> NAH
        //virtual shader_program_ptr_type make_shader_program(const build_in_shader_program) const = 0;

        //! gets a lazily instantiated shader provided by the context implementation
        virtual built_in_shader_ptr_type get_alpha_cutoff_shader() const = 0;
        //! gets a lazily instantiated shader provided by the context implementation
        virtual built_in_shader_ptr_type get_pink_shader_of_death() const = 0;

        //! gets a lazily instantiated model provided by the context implementation
        virtual built_in_model_ptr_type get_cube_model() const = 0;

        //! gets a lazily instantiated model provided by the context implementation
        //virtual built_in_model_ptr_type get_quad_model() const = 0;

        //! virtual destructor
        virtual ~context() = default;

    protected:
        //! prevent instantiation; context is the interface to all context implementations
        context() = default;
    };
}

#endif
