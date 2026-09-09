// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/exception.h>
#include <gdk/graphics/webgl1es2_material.h>

#include <iostream>

using namespace gdk;
using namespace gdk::graphics;

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
    throw exception("unhandled faceculling mode");
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
    throw exception("unhandled render mode");
}

webgl1es2_material::webgl1es2_material(shader_ptr_type pShader,
    material::face_culling_mode aface_culling_mode,
    material::render_mode aRenderMode)
: m_pShaderProgram(pShader)
, m_FaceCullMode(aface_culling_mode)
, m_RenderMode(aRenderMode)
{}

material::render_mode webgl1es2_material::get_render_mode() const {
    return m_RenderMode;
}

void webgl1es2_material::activate(gl_state &aState) {
    setface_culling_mode(m_FaceCullMode);
    setRenderMode(m_RenderMode);
	m_pShaderProgram->useProgram(aState);

    const auto upload = [this](const auto &aCollection) {
        for (const auto &[name, each] : aCollection)
            m_pShaderProgram->set_uniform(each.location, each.value);
    };

    upload(m_Integers);
    upload(m_Integer2s);
    upload(m_Integer3s);
    upload(m_Integer4s);

    upload(m_Floats);
    upload(m_Vector2s);
    upload(m_vector3s);
    upload(m_vector4s);

    upload(m_IntVector2Arrays);

	for (const auto& [name, a] : m_Textures) m_pShaderProgram->try_set_uniform(name, *a, aState);
}

webgl1es2_material::shader_ptr_type webgl1es2_material::getShaderProgram() {
    return m_pShaderProgram;
}

void webgl1es2_material::set_texture(const std::string_view aName, const texture_ptr_type aValue) {
    m_Textures[std::string(aName)] = std::static_pointer_cast<webgl1es2_texture>(aValue);
}

void webgl1es2_material::set_float(const std::string_view aName, float aValue) {
    assign(m_Floats, aName, aValue);
}

void webgl1es2_material::set_vector2(const std::string_view aName, vector2_type aValue) {
    assign(m_Vector2s, aName, aValue);
}

void webgl1es2_material::set_vector3(const std::string_view aName, vector3_type aValue) {
    assign(m_vector3s, aName, aValue);
}

void webgl1es2_material::set_vector4(const std::string_view aName, vector4_type aValue) {
    assign(m_vector4s, aName, aValue);
}

void webgl1es2_material::set_vector4(const std::string_view aName, const color &aValue) {
    assign(m_vector4s, aName, vector4_type(aValue.r, aValue.g, aValue.b, aValue.a));
}

void webgl1es2_material::set_integer(const std::string_view aName, int aValue) {
    assign(m_Integers, aName, aValue);
}

void webgl1es2_material::set_integer2(const std::string_view aName, int aValue1, int aValue2) {
    assign(m_Integer2s, aName, intvector2_type(aValue1, aValue2));
}

void webgl1es2_material::set_integer3(const std::string_view aName, int aValue1, int aValue2, int aValue3) {
    assign(m_Integer3s, aName, intvector3_type(aValue1, aValue2, aValue3));
}

void webgl1es2_material::set_integer4(const std::string_view aName, int aValue1, int aValue2, int aValue3, int aValue4) {
    assign(m_Integer4s, aName, intvector4_type(aValue1, aValue2, aValue3, aValue4));
}

void webgl1es2_material::set_int_vector2_array(const std::string_view aName, const std::vector<intvector2_type> &aValue) {
    assign(m_IntVector2Arrays, aName, aValue);
}

