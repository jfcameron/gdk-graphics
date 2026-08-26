// © Joseph Cameron - All Rights Reserved

#include <cmath>
#include <algorithm>
#include <gdk/graphics/glh.h>
#include <gdk/matrix4x4.h>
#include <gdk/graphics/opengl.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <jfc/default_ptr.h>

#include <iostream>

using namespace gdk;
using namespace gdk::graphics;

webgl1es2_entity::webgl1es2_entity(const std::shared_ptr<webgl1es2_model> amodel, const std::shared_ptr<webgl1es2_material> aMaterial)
: m_model(amodel)
, m_Material(aMaterial)
{}

void webgl1es2_entity::draw(const matrix4x4_type &aViewMatrix, const matrix4x4_type &aProjectionMatrix) const {
    if (m_IsHidden) return;

    const matrix4x4_type p = aProjectionMatrix;
    const matrix4x4_type v = aViewMatrix;
    const matrix4x4_type m = getModelMatrix();

    const auto mvp = p * v * m;
    
    m_Material->getShaderProgram()->set_standard_matrices(m, v, p, mvp);

    m_model->draw();
}

void webgl1es2_entity::world_bounds(vector3_type &aCentreOut,
    floating_point_type &aRadiusOut) const {
    const auto pModel = std::static_pointer_cast<webgl1es2_model>(m_model);

    const auto &local = pModel->bounds_centre();

    aCentreOut = {
        m_ModelMatrix.get(0, 0) * local.x + m_ModelMatrix.get(1, 0) * local.y
            + m_ModelMatrix.get(2, 0) * local.z + m_ModelMatrix.get(3, 0),
        m_ModelMatrix.get(0, 1) * local.x + m_ModelMatrix.get(1, 1) * local.y
            + m_ModelMatrix.get(2, 1) * local.z + m_ModelMatrix.get(3, 1),
        m_ModelMatrix.get(0, 2) * local.x + m_ModelMatrix.get(1, 2) * local.y
            + m_ModelMatrix.get(2, 2) * local.z + m_ModelMatrix.get(3, 2)};

    const auto axis_scale = [this](const std::size_t aColumn) {
        const auto x = m_ModelMatrix.get(aColumn, 0);
        const auto y = m_ModelMatrix.get(aColumn, 1);
        const auto z = m_ModelMatrix.get(aColumn, 2);

        return std::sqrt(x * x + y * y + z * z);
    };

    aRadiusOut = pModel->bounds_radius()
        * std::max(axis_scale(0), std::max(axis_scale(1), axis_scale(2)));
}

const matrix4x4_type &webgl1es2_entity::getModelMatrix() const {
    return m_ModelMatrix;
}

void webgl1es2_entity::set_transform(const vector3_type &aWorldPos, const quaternion_type &aRotation, const vector3_type &aScale) {
    m_ModelMatrix.set_to_identity();
    m_ModelMatrix.set_translation(aWorldPos);
    m_ModelMatrix.set_rotation_and_scale(aRotation, aScale);
}

void webgl1es2_entity::set_transform(const matrix4x4_type& a) {
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
