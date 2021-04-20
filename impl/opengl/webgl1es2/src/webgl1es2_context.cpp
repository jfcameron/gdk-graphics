// © 2019 Joseph Cameron - All Rights Reserved

#include <stdexcept>
#include <iostream>

#include <gdk/webgl1es2_screen_camera.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_scene.h>
#include <gdk/webgl1es2_shader_program.h>
#include <gdk/webgl1es2_texture_camera.h>

using namespace gdk;

webgl1es2_context::webgl1es2_context()
{}

std::shared_ptr<screen_camera> webgl1es2_context::make_camera() const 
{
    return std::shared_ptr<screen_camera>(new webgl1es2_screen_camera());
}

std::shared_ptr<screen_camera> webgl1es2_context::make_camera(const camera &other) const
{
    return std::shared_ptr<screen_camera>(
        new webgl1es2_screen_camera(static_cast<const webgl1es2_screen_camera &>(other)));
}

std::shared_ptr<texture_camera> webgl1es2_context::make_texture_camera() const
{
    return std::shared_ptr<texture_camera>(new webgl1es2_texture_camera());
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

graphics::context::material_ptr_type webgl1es2_context::make_material(
    gdk::graphics::context::shader_program_shared_ptr_type pShader,
    material::render_mode aRenderMode,
    material::FaceCullingMode aFaceCullingMode) const 
{
    return graphics::context::material_ptr_type(
        new webgl1es2_material(
            std::static_pointer_cast<webgl1es2_shader_program>(pShader),
            aFaceCullingMode,
            aRenderMode));
}

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
    return graphics::context::texture_ptr_type(new webgl1es2_texture(imageView));
}

graphics::context::texture_ptr_type webgl1es2_context::make_texture(const std::vector<std::underlying_type<std::byte>::type>& aRGBA32PNGData) const
{
    return std::make_unique<gdk::webgl1es2_texture>(webgl1es2_texture::make_from_png_rgba32(aRGBA32PNGData));
}

graphics::context::model_ptr_type webgl1es2_context::make_model(const vertex_data &vertexDataView) const
{
    return graphics::context::model_ptr_type(new gdk::webgl1es2_model(vertexDataView));
}

graphics::context::scene_ptr_type webgl1es2_context::make_scene() const
{
    return graphics::context::scene_ptr_type(
        new gdk::webgl1es2_scene());
}

