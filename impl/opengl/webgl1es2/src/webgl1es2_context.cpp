// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics_exception.h>
#include <gdk/model_data.h>
#include <gdk/texture_data.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_scene.h>
#include <gdk/webgl1es2_screen_camera.h>
#include <gdk/webgl1es2_shader_program.h>
#include <gdk/webgl1es2_texture_camera.h>

#include <iostream>
#include <stdexcept>

using namespace gdk;

static int sInstanceCount(0);

context_ptr_type webgl1es2_context::make() {
    if (sInstanceCount) throw graphics_exception(
        "webgl1es2_context does not support multiple instances since OpenGL's " 
        "global state would cause them to interfere with one another");

    return context_ptr_type(new webgl1es2_context());
}

webgl1es2_context::webgl1es2_context() { ++sInstanceCount; }

webgl1es2_context::webgl1es2_context(webgl1es2_context &&) { ++sInstanceCount; }

webgl1es2_context::~webgl1es2_context() { --sInstanceCount; }

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
                std::const_pointer_cast<gdk::model>(pModel)),
            std::static_pointer_cast<webgl1es2_material>(
                std::const_pointer_cast<gdk::material>(pMaterial)
        )));
}

material_ptr_type webgl1es2_context::make_material(
    const const_shader_ptr_type pShader,
    const material::render_mode aRenderMode,
    const material::face_culling_mode aFaceCullingMode) {
    return material_ptr_type(
        new webgl1es2_material(
            std::static_pointer_cast<webgl1es2_shader_program>(
                std::const_pointer_cast<gdk::shader_program>(pShader)),
            aFaceCullingMode,
            aRenderMode));
}

shader_ptr_type webgl1es2_context::make_shader(
    const std::string_view aVertexShaderStageSourceCodeGLSL, 
    const std::string_view aFragmentShaderStageSourceCodeGLSL) {
    return shader_ptr_type(
        new webgl1es2_shader_program(std::string(aVertexShaderStageSourceCodeGLSL), std::string(aFragmentShaderStageSourceCodeGLSL)));
}

shader_ptr_type webgl1es2_context::get_alpha_cutoff_shader() const {
    return std::static_pointer_cast<shader_program>(
        static_cast<std::shared_ptr<webgl1es2_shader_program>>(
            webgl1es2_shader_program::AlphaCutOff));
}

model_ptr_type webgl1es2_context::get_cube_model() const {
    return std::static_pointer_cast<model>(
        std::shared_ptr<webgl1es2_model>(webgl1es2_model::Cube));
}

model_ptr_type webgl1es2_context::get_quad_model() const {
    return std::static_pointer_cast<model>(
        std::shared_ptr<webgl1es2_model>(webgl1es2_model::Quad));
}

texture_ptr_type webgl1es2_context::make_texture(const texture_data::view &imageView, const texture::wrap_mode aWrapModeU,
    const texture::wrap_mode aWrapModeV) {
    return texture_ptr_type(new webgl1es2_texture(imageView, aWrapModeU, aWrapModeV));
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
    return model_ptr_type(new gdk::webgl1es2_model(
        gdk::model::usage_hint::upload_once, vertexDataView));
}

model_ptr_type webgl1es2_context::make_model() {
    //TODO: should support not having a vbo at all. this works but leads to allocating tiny vbos for "empty" models
    return make_model(model::usage_hint::upload_once, {{
        { "nil", { {
            0.0f, 0.0f, 0.0f,
        }, 1 } },
    }});
}

scene_ptr_type webgl1es2_context::make_scene() {
    return scene_ptr_type(new gdk::webgl1es2_scene());
}

