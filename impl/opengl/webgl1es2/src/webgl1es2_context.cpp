// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_context.h>

#include <gdk/graphics/opengl.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_scene.h>
#include <gdk/graphics/webgl1es2_screen_camera.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <iostream>
#include <stdexcept>

using namespace gdk;
using namespace gdk::graphics;

context_ptr_type webgl1es2_context::make() {
    return context_ptr_type(new webgl1es2_context());
}

webgl1es2_context::webgl1es2_context()
: m_pState(std::make_shared<gl_state>()) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
}

webgl1es2_context::webgl1es2_context(webgl1es2_context &&) = default;

webgl1es2_context::~webgl1es2_context() = default;

std::shared_ptr<gl_state> webgl1es2_context::state() const { return m_pState; }

std::shared_ptr<screen_camera> webgl1es2_context::make_camera() {
    return std::shared_ptr<screen_camera>(new webgl1es2_screen_camera());
}

std::shared_ptr<texture_camera> webgl1es2_context::make_texture_camera() {
    return std::shared_ptr<texture_camera>(new webgl1es2_texture_camera());
}

entity_ptr_type webgl1es2_context::make_entity(
    const const_model_ptr_type pModel, 
    const const_material_ptr_type pMaterial) {
    return entity_ptr_type(
        new webgl1es2_entity(
            std::static_pointer_cast<webgl1es2_model>(
                std::const_pointer_cast<gdk::graphics::model>(pModel)),
            std::static_pointer_cast<webgl1es2_material>(
                std::const_pointer_cast<gdk::graphics::material>(pMaterial)
        )));
}

material_ptr_type webgl1es2_context::make_material(
    const const_shader_ptr_type pShader,
    const material::render_mode aRenderMode,
    const material::face_culling_mode aFaceCullingMode) {
    return material_ptr_type(
        new webgl1es2_material(
            std::static_pointer_cast<webgl1es2_shader_program>(
                std::const_pointer_cast<gdk::graphics::shader_program>(pShader)),
            aFaceCullingMode,
            aRenderMode));
}

shader_ptr_type webgl1es2_context::make_shader(
    const std::string_view aVertexShaderStageSourceCodeGLSL, 
    const std::string_view aFragmentShaderStageSourceCodeGLSL) {
    return shader_ptr_type(
        new webgl1es2_shader_program(std::string(aVertexShaderStageSourceCodeGLSL), std::string(aFragmentShaderStageSourceCodeGLSL)));
}

shader_ptr_type webgl1es2_context::make_alpha_cutoff_shader() const {
    return std::static_pointer_cast<shader_program>(webgl1es2_shader_program::make_alpha_cutoff());
}

model_ptr_type webgl1es2_context::make_cube_model() const {
    return std::static_pointer_cast<model>(webgl1es2_model::make_cube());
}

model_ptr_type webgl1es2_context::make_sphere_model() const {
    return std::static_pointer_cast<model>(webgl1es2_model::make_sphere());
}

texture_ptr_type webgl1es2_context::make_texture(const texture_data::view &imageView, const texture::wrap_mode aWrapModeU,
    const texture::wrap_mode aWrapModeV, const texture::filter_mode aFilterMode) {
    const auto sharp = aFilterMode == texture::filter_mode::sharp;

    return texture_ptr_type(new webgl1es2_texture(imageView, aWrapModeU, aWrapModeV,
        sharp ? webgl1es2_texture::minification_filter::nearest
              : webgl1es2_texture::minification_filter::linear,
        sharp ? webgl1es2_texture::magnification_filter::nearest
              : webgl1es2_texture::magnification_filter::linear));
}

texture_ptr_type webgl1es2_context::make_texture() {
    texture_data::view view;
    view.width = 0;
    view.height = 0;
    view.format = texture::format::grey;
    view.data = nullptr;

    return texture_ptr_type(new webgl1es2_texture(view));
}

model_ptr_type webgl1es2_context::make_model(const model::usage_hint usage,
    const model_data &vertexDataView) {
    return model_ptr_type(new gdk::graphics::webgl1es2_model(usage, vertexDataView));
}

model_ptr_type webgl1es2_context::make_model() {
    //TODO: Possibly delay initial vbo upload until user-data is actually provided.
    return make_model(model::usage_hint::upload_once, {{
        { "nil", { {
            0.0f, 0.0f, 0.0f,
        }, 1 } },
    }});
}

scene_ptr_type webgl1es2_context::make_scene() {
    return scene_ptr_type(new gdk::graphics::webgl1es2_scene(m_pState));
}

