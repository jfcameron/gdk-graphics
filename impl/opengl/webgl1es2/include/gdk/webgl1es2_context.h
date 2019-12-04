// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_CONTEXT_H
#define GDK_GFX_WEBGL1ES2_CONTEXT_H

#include <gdk/graphics_context.h>

namespace gdk
{
    //! brief webgl1/gles2.0 context implementation
    class webgl1es2_context final : public graphics::context
    {
    public:
        virtual graphics::context::camera_ptr_type make_camera() const override;
        //virtual graphics::context::entity_ptr_type make_entity() const override;
        //virtual graphics::context::material_ptr_type make_material() const override;
        //virtual graphics::context::model_ptr_type make_model() const override;

        //virtual graphics::context::shader_program_ptr_type make_shader_program(const build_in_shader_program aBuildInShader) const override;

        virtual graphics::context::built_in_shader_ptr_type get_alpha_cutoff_shader() const override;
        virtual built_in_shader_ptr_type get_pink_shader_of_death() const override;

        virtual built_in_model_ptr_type get_cube_model() const override;

        //! default ctor
        webgl1es2_context(); 

        virtual ~webgl1es2_context() override = default;
    };
}

#endif
