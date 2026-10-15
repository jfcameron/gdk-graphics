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

context_ptr_type webgl1es2_context::make(std::optional<graphics::profile> aProfile) {
    return context_ptr_type(new webgl1es2_context(std::move(aProfile)));
}

namespace {
    [[nodiscard]] std::size_t gl_integer(const GLenum aWhich) {
        GLint value = 0;
        glGetIntegerv(aWhich, &value);

        return static_cast<std::size_t>(value < 0 ? 0 : value);
    }

    [[nodiscard]] graphics::limits limits_of_gl() {
        graphics::limits out;
        out.texture_size = static_cast<std::size_t>(webgl1es2_texture::getMaxTextureSize());
        out.non_power_of_two_textures = false;  
        out.texture_units = gl_integer(GL_MAX_TEXTURE_IMAGE_UNITS);
        out.vertex_texture_units = gl_integer(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
        out.vertex_attributes = gl_integer(GL_MAX_VERTEX_ATTRIBS);
#ifdef GL_MAX_VERTEX_UNIFORM_VECTORS
        out.vertex_uniform_vectors = gl_integer(GL_MAX_VERTEX_UNIFORM_VECTORS);
        out.fragment_uniform_vectors = gl_integer(GL_MAX_FRAGMENT_UNIFORM_VECTORS);
        out.varying_vectors = gl_integer(GL_MAX_VARYING_VECTORS);
#else
        out.vertex_uniform_vectors = gl_integer(GL_MAX_VERTEX_UNIFORM_COMPONENTS) / 4;
        out.fragment_uniform_vectors = gl_integer(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS) / 4;
        out.varying_vectors = gl_integer(GL_MAX_VARYING_FLOATS) / 4;
#endif
        return out;
    }
}

webgl1es2_context::webgl1es2_context(std::optional<graphics::profile> aProfile)
: m_pState(std::make_shared<gl_state>())
, m_Limits(limits_of_gl())
, m_pProfile(aProfile ? std::make_shared<const graphics::profile>(std::move(*aProfile)) : nullptr) {
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

std::shared_ptr<texture_camera> webgl1es2_context::make_texture_camera(const intvector2_type &aSize) {
    return std::shared_ptr<texture_camera>(new webgl1es2_texture_camera(aSize));
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

material_ptr_type webgl1es2_context::make_material(const const_material_ptr_type &aPrototype) {
    if (!aPrototype) throw exception("webgl1es2_context::make_material: no prototype to copy");

    return std::make_shared<webgl1es2_material>(
        *std::static_pointer_cast<const webgl1es2_material>(aPrototype));
}

shader_ptr_type webgl1es2_context::make_shader(
    const std::string_view aVertexShaderStageSourceCodeGLSL, 
    const std::string_view aFragmentShaderStageSourceCodeGLSL) {
    if (m_pProfile) check_shader(*m_pProfile, aVertexShaderStageSourceCodeGLSL, aFragmentShaderStageSourceCodeGLSL);

    return shader_ptr_type(
        new webgl1es2_shader_program(std::string(aVertexShaderStageSourceCodeGLSL), std::string(aFragmentShaderStageSourceCodeGLSL)));
}

graphics::limits webgl1es2_context::limits() const {
    return m_pProfile ? narrowest(m_Limits, m_pProfile->most) : m_Limits;
}

shader_ptr_type webgl1es2_context::make_alpha_blend_shader() const {
    return std::static_pointer_cast<shader_program>(webgl1es2_shader_program::make_alpha_blend());
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
    if (m_pProfile) check_texture(*m_pProfile, imageView.width, imageView.height);

    const auto sharp = aFilterMode == texture::filter_mode::sharp;

    const auto pTexture = std::make_shared<webgl1es2_texture>(imageView, aWrapModeU, aWrapModeV,
        sharp ? webgl1es2_texture::minification_filter::nearest
              : webgl1es2_texture::minification_filter::linear,
        sharp ? webgl1es2_texture::magnification_filter::nearest
              : webgl1es2_texture::magnification_filter::linear);
    pTexture->hold_to(m_pProfile);

    return pTexture;
}

texture_ptr_type webgl1es2_context::make_texture() {
    texture_data::view view;
    view.width = 0;
    view.height = 0;
    view.format = texture::format::grey;
    view.data = nullptr;

    const auto pTexture = std::make_shared<webgl1es2_texture>(view);
    pTexture->hold_to(m_pProfile);

    return pTexture;
}

model_ptr_type webgl1es2_context::make_model(const model::usage_hint usage,
    const model_data &vertexDataView) {
    if (m_pProfile) check_model(*m_pProfile, vertexDataView);

    const auto pModel = std::make_shared<webgl1es2_model>(usage, vertexDataView);
    pModel->hold_to(m_pProfile);

    return pModel;
}

model_ptr_type webgl1es2_context::make_model() {
    return make_model(model::usage_hint::upload_once, model_data());
}

scene_ptr_type webgl1es2_context::make_scene() {
    return scene_ptr_type(new gdk::graphics::webgl1es2_scene(m_pState));
}

