// © Joseph Cameron - All Rights Reserved

#include <gdk/texture_data.h>
#include <gdk/vertex_data.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_scene.h>
#include <gdk/webgl1es2_screen_camera.h>
#include <gdk/webgl1es2_shader_program.h>
#include <gdk/webgl1es2_texture_camera.h>

#include <stdexcept>
#include <iostream>

using namespace gdk;

webgl1es2_context::webgl1es2_context()
{}

std::shared_ptr<screen_camera> webgl1es2_context::make_camera() const {
    return std::shared_ptr<screen_camera>(new webgl1es2_screen_camera());
}

std::shared_ptr<texture_camera> webgl1es2_context::make_texture_camera() const {
    return std::shared_ptr<texture_camera>(new webgl1es2_texture_camera());
}

graphics::context::entity_ptr_type webgl1es2_context::make_entity(gdk::graphics::context::model_ptr_type pModel, 
    gdk::graphics::context::material_ptr_type pMaterial) const {
    return graphics::context::entity_ptr_type(
        new webgl1es2_entity(
            std::static_pointer_cast<webgl1es2_model>(pModel),
            std::static_pointer_cast<webgl1es2_material>(pMaterial)));
}

graphics::context::material_ptr_type webgl1es2_context::make_material(
    gdk::graphics::context::shader_ptr_type pShader,
    material::render_mode aRenderMode,
    material::FaceCullingMode aFaceCullingMode) const {
    return graphics::context::material_ptr_type(
        new webgl1es2_material(
            std::static_pointer_cast<webgl1es2_shader_program>(pShader),
            aFaceCullingMode,
            aRenderMode));
}

graphics::context::shader_ptr_type webgl1es2_context::make_shader(const std::string &aVertexShaderStageSourceCodeGLSL, const std::string &aFragmentShaderStageSourceCodeGLSL) const {
    return graphics::context::shader_ptr_type(
        new webgl1es2_shader_program(aVertexShaderStageSourceCodeGLSL, aFragmentShaderStageSourceCodeGLSL));
}

graphics::context::shader_ptr_type webgl1es2_context::get_alpha_cutoff_shader() const {
    return std::static_pointer_cast<shader_program>(
        static_cast<std::shared_ptr<webgl1es2_shader_program>>(
            webgl1es2_shader_program::AlphaCutOff));
}

graphics::context::shader_ptr_type webgl1es2_context::get_pink_shader_of_death() const {
    return std::static_pointer_cast<shader_program>(
        static_cast<std::shared_ptr<webgl1es2_shader_program>>(
            webgl1es2_shader_program::PinkShaderOfDeath));
}

graphics::context::model_ptr_type webgl1es2_context::get_cube_model() const {
    return std::static_pointer_cast<model>(
        std::shared_ptr<webgl1es2_model>(webgl1es2_model::Cube));
}

graphics::context::model_ptr_type webgl1es2_context::get_quad_model() const {
    return std::static_pointer_cast<model>(
        std::shared_ptr<webgl1es2_model>(webgl1es2_model::Quad));
}

graphics::context::texture_ptr_type webgl1es2_context::make_texture(const texture_data::view &imageView, const texture::wrap_mode aWrapModeU,
    const texture::wrap_mode aWrapModeV) const
{
    return graphics::context::texture_ptr_type(
        new webgl1es2_texture(imageView, aWrapModeU, aWrapModeV));
}

graphics::context::texture_ptr_type webgl1es2_context::make_texture() const {
    texture_data::view view;
    view.width = 0;
    view.height = 0;
    view.format = texture::format::grey;
    view.data = nullptr;

    return graphics::context::texture_ptr_type(new webgl1es2_texture(view));
}

graphics::context::model_ptr_type webgl1es2_context::make_model(const model::usage_hint usage,
    const vertex_data &vertexDataView) const {
    return graphics::context::model_ptr_type(new gdk::webgl1es2_model(
        gdk::model::usage_hint::write_once, 
        vertexDataView));
}

graphics::context::model_ptr_type webgl1es2_context::make_model() const {
    std::vector<float> data({0, 0, 0});
    return make_model(model::usage_hint::write_once, 
        {{{"a_Position", {data, 1}}}});
}

graphics::context::scene_ptr_type webgl1es2_context::make_scene() const {
    return graphics::context::scene_ptr_type(
        new gdk::webgl1es2_scene());
}

graphics::context::context_ptr_type webgl1es2_context::make() {
    return std::make_unique<webgl1es2_context>(webgl1es2_context());
}

