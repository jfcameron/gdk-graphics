// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/webgl1es2_camera.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_shader_program.h>

using namespace gdk;

webgl1es2_context::webgl1es2_context()
{}

graphics::context::camera_ptr_type webgl1es2_context::make_camera() const 
{
    return graphics::context::camera_ptr_type(new webgl1es2_camera());
}

/*graphics::context::entity_ptr_type webgl1es2_context::make_entity() const 
{
    return graphics::context::entity_ptr_type(new webgl1es2_entity());
}*/

/*graphics::context::material_ptr_type webgl1es2_context::make_material() const 
{
    return std::make_unique<material>(webgl1es2_material());
}*/

/*graphics::context::model_ptr_type webgl1es2_context::make_model() const 
{
    return std::make_unique<model>(webgl1es2_model());
}*/

/*graphics::context::shader_program_ptr_type webgl1es2_context::make_shader_program(const graphics::context::built_in_shader_program aBuildInShader) const 
{
}*/

graphics::context::built_in_shader_ptr_type webgl1es2_context::get_alpha_cutoff_shader() const
{
    return std::static_pointer_cast<shader_program>(
        static_cast<std::shared_ptr<webgl1es2_shader_program>>(
            webgl1es2_shader_program::AlphaCutOff));
}

graphics::context::built_in_shader_ptr_type webgl1es2_context::get_pink_shader_of_death() const
{
    return std::static_pointer_cast<shader_program>(
        static_cast<std::shared_ptr<webgl1es2_shader_program>>(
            webgl1es2_shader_program::PinkShaderOfDeath));
}

graphics::context::built_in_model_ptr_type webgl1es2_context::get_cube_model() const
{
    return std::static_pointer_cast<model>(
        std::shared_ptr<webgl1es2_model>(webgl1es2_model::Cube));
}

