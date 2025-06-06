// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/matrix4x4.h>
#include <gdk/opengl.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_shader_program.h>
#include <jfc/default_ptr.h>

#include <iostream>

using namespace gdk;

static constexpr char TAG[] = "entity";

webgl1es2_entity::webgl1es2_entity(const std::shared_ptr<webgl1es2_model> amodel, const std::shared_ptr<webgl1es2_material> aMaterial)
: m_model(amodel)
, m_Material(aMaterial)
{}

void webgl1es2_entity::draw(const graphics_matrix4x4_type &aViewMatrix, const graphics_matrix4x4_type &aProjectionMatrix) const {
    if (m_IsHidden) return;

    const graphics_matrix4x4_type p = aProjectionMatrix;
    const graphics_matrix4x4_type v = aViewMatrix;
    const graphics_matrix4x4_type m = getModelMatrix();

    const auto mvp = p * v * m;
    
    auto pShader = m_Material->getShaderProgram();
    pShader->try_set_uniform("_Model", m); 
    pShader->try_set_uniform("_View", v);
    pShader->try_set_uniform("_Projection", p);
    pShader->try_set_uniform("_MVP", mvp);

    m_model->draw();
}

const graphics_matrix4x4_type &webgl1es2_entity::getModelMatrix() const {
    return m_ModelMatrix;
}

void webgl1es2_entity::set_transform(const graphics_vector3_type &aWorldPos, const graphics_quaternion_type &aRotation, const graphics_vector3_type &aScale) {
    m_ModelMatrix.set_to_identity();
    m_ModelMatrix.set_translation(aWorldPos);
    m_ModelMatrix.set_rotation(aRotation, aScale);
}

void webgl1es2_entity::set_transform(const graphics_matrix4x4_type& a) {
	m_ModelMatrix = a;
}

std::shared_ptr<model> webgl1es2_entity::getModel() const {
    return m_model;
}

std::shared_ptr<material> webgl1es2_entity::getMaterial() const {
    return m_Material;
}

void webgl1es2_entity::hide() {
    m_IsHidden = true;
}

void webgl1es2_entity::show() {
    m_IsHidden = false;
}

bool webgl1es2_entity::is_hidden() const {
    return m_IsHidden;
}
