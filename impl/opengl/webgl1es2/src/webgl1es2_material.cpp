// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics_exception.h>
#include <gdk/webgl1es2_material.h>

#include <iostream>

using namespace gdk;

static inline void setface_culling_mode(const material::face_culling_mode a) {
    if (a == material::face_culling_mode::none) {
        glDisable(GL_CULL_FACE);
        return;
    }

    glEnable(GL_CULL_FACE);
    switch(a) {
        case material::face_culling_mode::front: glCullFace(GL_FRONT); return;
        case material::face_culling_mode::back: glCullFace(GL_BACK); return;
        case material::face_culling_mode::front_and_back: glCullFace(GL_FRONT_AND_BACK); return;
        default: break;
    }
    throw graphics_exception("unhandled faceculling mode");
}

static inline void setRenderMode(const material::render_mode aRenderMode) {
    if (aRenderMode == material::render_mode::opaque) {
        glDisable(GL_BLEND);
        return;
    }

    glEnable(GL_BLEND);
    switch(aRenderMode) {
        case material::render_mode::transparent: 
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); return;
        default: break;
    }
    throw graphics_exception("unhandled render mode");
}

webgl1es2_material::webgl1es2_material(graphics_shader_ptr_type pShader,
    material::face_culling_mode aface_culling_mode,
    material::render_mode aRenderMode)
: m_pShaderProgram(pShader)
, m_FaceCullMode(aface_culling_mode)
, m_RenderMode(aRenderMode)
{}

material::render_mode webgl1es2_material::get_render_mode() const {
    return m_RenderMode;
}

void webgl1es2_material::activate() {
    setface_culling_mode(m_FaceCullMode);
    setRenderMode(m_RenderMode);
	m_pShaderProgram->useProgram();

	for (const auto& [name, a] : m_Integers) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Integer2s) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Integer3s) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Integer4s) m_pShaderProgram->try_set_uniform(name, a);
   
    for (const auto& [name, a] : m_Floats) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Vector2s) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_vector3s) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_vector4s) m_pShaderProgram->try_set_uniform(name, a);

	for (const auto& [name, a] : m_Textures) m_pShaderProgram->try_set_uniform(name, *a);
}

webgl1es2_material::graphics_shader_ptr_type webgl1es2_material::getShaderProgram() {
    return m_pShaderProgram;
}

void webgl1es2_material::set_texture(const std::string_view aName, const graphics_texture_ptr_type aValue) {
    m_Textures[std::string(aName)] = std::static_pointer_cast<webgl1es2_texture>(aValue);
}

void webgl1es2_material::set_float(const std::string_view aName, float aValue) {
    m_Floats[std::string(aName)] = aValue;
}

void webgl1es2_material::set_vector2(const std::string_view aName, graphics_vector2_type aValue) {
    m_Vector2s[std::string(aName)] = aValue;
}

void webgl1es2_material::set_vector3(const std::string_view aName, graphics_vector3_type aValue) {
    m_vector3s[std::string(aName)] = aValue;
}

void webgl1es2_material::set_vector4(const std::string_view aName, graphics_vector4_type aValue) {
    m_vector4s[std::string(aName)] = aValue;
}

void webgl1es2_material::set_integer(const std::string_view aName, int aValue) {
    m_Integers[std::string(aName)] = aValue;
}

void webgl1es2_material::set_integer2(const std::string_view aName, int aValue1, int aValue2) {
    m_Integer2s[std::string(aName)] = {aValue1, aValue2};
}

void webgl1es2_material::set_integer3(const std::string_view aName, int aValue1, int aValue2, int aValue3) {
    m_Integer3s[std::string(aName)] = {aValue1, aValue2, aValue3};
}

void webgl1es2_material::set_integer4(const std::string_view aName, int aValue1, int aValue2, int aValue3, int aValue4) {
    m_Integer4s[std::string(aName)] = {aValue1, aValue2, aValue3, aValue4};
}

void webgl1es2_material::set_int_vector2_array(const std::string_view aName, const std::vector<graphics_intvector2_type> &aValue) {
    m_IntVector2Arrays[std::string(aName)] = {aValue};
}

