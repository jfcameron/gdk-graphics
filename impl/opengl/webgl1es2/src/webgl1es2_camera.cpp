// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/glh.h>
#include <gdk/graphics/exception.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_camera.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include <cmath>

using namespace gdk;
using namespace gdk::graphics;

webgl1es2_camera::webgl1es2_camera() = default;

void webgl1es2_camera::set_clear_color(const gdk::graphics::color& acolor) {
    m_ClearColor = acolor;
}

void webgl1es2_camera::set_clear_mode(const camera::clear_mode aClearMode) {
    m_ClearMode = aClearMode;
}

void webgl1es2_camera::set_transform(const gdk::graphics::vector3_type &aWorldPos, const gdk::graphics::quaternion_type &aRotation) {
    m_WorldMatrix.set_to_identity();
    m_WorldMatrix.set_translation(aWorldPos);
    m_WorldMatrix.set_rotation(aRotation); 

    m_ViewMatrix.set_to_identity();
    m_ViewMatrix.set_rotation(aRotation.inverse_unit());

    matrix4x4_type matrixPosition;
    matrixPosition.set_translation(aWorldPos * -1.f);

    m_ViewMatrix *= matrixPosition;
}

void webgl1es2_camera::set_transform(const gdk::graphics::matrix4x4_type &aMatrix) {
    set_transform(aMatrix.translation(), aMatrix.rotation());
}

namespace {
    [[nodiscard]] gdk::graphics::matrix4x4_type make_gl_perspective(const float aFieldOfView,
        const float aNearClippingPlane, const float aFarClippingPlane,
        const float aViewportAspectRatio) {
        const auto tanHalfFovy = static_cast<float>(std::tan(aFieldOfView * 0.5));

        gdk::graphics::matrix4x4_type matrix;

        matrix.set(0, 0, static_cast<float>(1.0 / (aViewportAspectRatio * tanHalfFovy)));
        matrix.set(0, 1, 0.0);
        matrix.set(0, 2, 0.0);
        matrix.set(0, 3, 0.0);

        matrix.set(1, 0, 0.0);
        matrix.set(1, 1, static_cast<float>(1.0 / tanHalfFovy));
        matrix.set(1, 2, 0.0);
        matrix.set(1, 3, 0.0);

        matrix.set(2, 0, 0.0);
        matrix.set(2, 1, 0.0);
        matrix.set(2, 2, -(aFarClippingPlane + aNearClippingPlane) / (aFarClippingPlane - aNearClippingPlane));
        matrix.set(2, 3, -1.0);

        matrix.set(3, 0, 0.0);
        matrix.set(3, 1, 0.0);
        matrix.set(3, 2, static_cast<float>(-2.0 * aFarClippingPlane * aNearClippingPlane / (aFarClippingPlane - aNearClippingPlane)));
        matrix.set(3, 3, 0.0);

        return matrix;
    }

    [[nodiscard]] gdk::graphics::matrix4x4_type make_gl_orthographic(
        const gdk::graphics::vector2_type &aOrthographicSize, const float aNearClippingPlane,
        const float aFarClippingPlane, const float aViewportAspectRatio) {
        const auto x = 2.0f / (aOrthographicSize.x * aViewportAspectRatio);
        const auto y = 2.0f / aOrthographicSize.y;
        const auto n = -(aFarClippingPlane + aNearClippingPlane) / (aFarClippingPlane - aNearClippingPlane);
        const auto f = -2.0f / (aFarClippingPlane - aNearClippingPlane);

        gdk::graphics::matrix4x4_type matrix;
        matrix.set(0, 0, x ); matrix.set(1, 0, 0.); matrix.set(2, 0, 0.); matrix.set(3, 0, 0.);
        matrix.set(0, 1, 0.); matrix.set(1, 1, y ); matrix.set(2, 1, 0.); matrix.set(3, 1, 0.);
        matrix.set(0, 2, 0.); matrix.set(1, 2, 0.); matrix.set(2, 2, f ); matrix.set(3, 2, n );
        matrix.set(0, 3, 0.); matrix.set(1, 3, 0.); matrix.set(2, 3, 0.); matrix.set(3, 3, 1.);

        return matrix;
    }
}

void webgl1es2_camera::set_perspective_projection(const float aFieldOfView,
    const float aNearClippingPlane, const float aFarClippingPlane,
    const float aViewportAspectRatio) {
    m_ProjectionMatrix = make_gl_perspective(aFieldOfView, aNearClippingPlane, aFarClippingPlane,
        aViewportAspectRatio);
}

void webgl1es2_camera::set_orthographic_projection(const gdk::graphics::vector2_type &aOrthographicSize,
    const float aNearClippingPlane, const float aFarClippingPlane,
    const float aViewportAspectRatio) {
    m_ProjectionMatrix = make_gl_orthographic(aOrthographicSize, aNearClippingPlane,
        aFarClippingPlane, aViewportAspectRatio);
}

const matrix4x4_type &webgl1es2_camera::get_view_matrix() const { 
    return m_ViewMatrix; 
}

const matrix4x4_type &webgl1es2_camera::get_world_matrix() const {
    return m_WorldMatrix;
}

const matrix4x4_type &webgl1es2_camera::get_projection_matrix() const { 
    return m_ProjectionMatrix; 
}

camera::clear_mode webgl1es2_camera::get_clearmode() const {
    return m_ClearMode;
}

gdk::graphics::color webgl1es2_camera::get_clearcolor() const {
    return m_ClearColor;
}

void webgl1es2_camera::activate_clear_mode() const {
    switch(m_ClearMode) {
        case gdk::graphics::camera::clear_mode::nothing: return;

        case gdk::graphics::camera::clear_mode::color_and_depth: {
            glh::ClearColor(m_ClearColor);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return;
        } 

        case gdk::graphics::camera::clear_mode::depth_only: {
            glClear(GL_DEPTH_BUFFER_BIT);
            return;
        } 

        default: break;
    }
    throw exception("unhandled clear mode");
}

