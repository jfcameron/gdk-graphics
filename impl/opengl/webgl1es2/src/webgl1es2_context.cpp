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

graphics::context::camera_ptr_type webgl1es2_context::make_camera(const camera &other) const
{
    return graphics::context::camera_ptr_type(
        new webgl1es2_camera(
            static_cast<const webgl1es2_camera &>(other)));
}

graphics::context::entity_ptr_type webgl1es2_context::make_entity(gdk::graphics::context::model_shared_ptr_type pModel, 
    gdk::graphics::context::material_shared_ptr_type pMaterial) const
{
    return graphics::context::entity_ptr_type(
        new webgl1es2_entity(
            std::static_pointer_cast<webgl1es2_model>(pModel),
            std::static_pointer_cast<webgl1es2_material>(pMaterial)));
}

graphics::context::entity_ptr_type webgl1es2_context::make_entity(const entity &other) const
{
    return graphics::context::entity_ptr_type(
        new webgl1es2_entity(
            static_cast<const webgl1es2_entity &>(other)));
}

graphics::context::material_ptr_type webgl1es2_context::make_material(gdk::graphics::context::shader_program_shared_ptr_type pShader) const 
{
    return graphics::context::material_ptr_type(
        new webgl1es2_material(
            std::static_pointer_cast<webgl1es2_shader_program>(pShader)));
}

/*graphics::context::model_ptr_type webgl1es2_context::make_model() const 
{
    return std::make_unique<model>(webgl1es2_model());
}*/

graphics::context::shader_program_ptr_type webgl1es2_context::make_shader(const std::string &aVertexGLSL, const std::string &aFragGLSL) const 
{
    return graphics::context::shader_program_ptr_type(
        new webgl1es2_shader_program(aVertexGLSL, aFragGLSL));
}

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

graphics::context::built_in_model_ptr_type webgl1es2_context::get_quad_model() const
{
    return std::static_pointer_cast<model>(
        std::shared_ptr<webgl1es2_model>(webgl1es2_model::Quad));
}

graphics::context::texture_ptr_type webgl1es2_context::make_texture(const texture::image_data_2d_view &imageView) const
{
    webgl1es2_texture::webgl1es2_texture_2d_data_view_type data;
    data.width = imageView.width;
    data.height = imageView.height;

    switch(imageView.format)
    {
        case texture::data_format::rgb: data.format = webgl1es2_texture::format::rgb; break;

        case texture::data_format::rgba: data.format = webgl1es2_texture::format::rgba; break;

        default: throw std::invalid_argument("webgl1es2 context does not support provided image format");
    }

    data.data = imageView.data;
    
    return graphics::context::texture_ptr_type(new webgl1es2_texture(data));
}

