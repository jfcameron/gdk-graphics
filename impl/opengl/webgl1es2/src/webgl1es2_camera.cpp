// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/graphics_exception.h>
#include <gdk/texture_data.h>
#include <gdk/webgl1es2_camera.h>
#include <gdk/webgl1es2_texture.h>

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>

using namespace gdk;

webgl1es2_camera::webgl1es2_camera() {
    static std::once_flag once;

    std::call_once(once, []() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
    });
}

void webgl1es2_camera::set_clear_color(const gdk::color& acolor) {
    m_ClearColor = acolor;
}

void webgl1es2_camera::set_clear_mode(const camera::clear_mode aClearMode) {
    m_ClearMode = aClearMode;
}

void webgl1es2_camera::set_transform(const gdk::graphics_vector3_type &aWorldPos, const gdk::graphics_quaternion_type &aRotation) {
    m_WorldMatrix.set_to_identity();
    m_WorldMatrix.set_translation(aWorldPos);
    m_WorldMatrix.set_rotation(aRotation); 

    m_ViewMatrix.set_to_identity();

    auto eulerRotation(aRotation.toEuler()); 
    graphics_quaternion_type negativeRotation{{ 
        eulerRotation.x * -1.f, 
        eulerRotation.y * -1.f, 
        eulerRotation.z // leaving Z unnegated is deliberate
    }};
    m_ViewMatrix.set_rotation(negativeRotation);

    graphics_mat4x4_type matrixPosition;
    matrixPosition.set_translation(aWorldPos * -1.f);

    m_ViewMatrix *= matrixPosition;
}

void webgl1es2_camera::set_transform(const gdk::graphics_mat4x4_type &aMatrix) {
    set_transform(aMatrix.translation(), aMatrix.rotation());
}

void webgl1es2_camera::set_perspective_projection(const float aFieldOfView,
    const float aNearClippingPlane, 
    const float aFarClippingPlane, 
    const float aViewportAspectRatio) {
    m_ProjectionMatrix.set_to_perspective(aFieldOfView, 
        aNearClippingPlane, aFarClippingPlane, aViewportAspectRatio);
}

void webgl1es2_camera::set_orthographic_projection(const float aWidth,
    const float aHeight,
    const float aNearClippingPlane,
    const float aFarClippingPlane,
    const float aViewportAspectRatio) {
    m_ProjectionMatrix.set_to_orthographic({ aWidth, aHeight }, 
        aNearClippingPlane, aFarClippingPlane, aViewportAspectRatio);
}

graphics_mat4x4_type webgl1es2_camera::get_view_matrix() const { 
    return m_ViewMatrix; 
}

graphics_mat4x4_type webgl1es2_camera::get_world_matrix() const {
    return m_WorldMatrix;
}

graphics_mat4x4_type webgl1es2_camera::get_projection_matrix() const { 
    return m_ProjectionMatrix; 
}

camera::clear_mode webgl1es2_camera::get_clearmode() const {
    return m_ClearMode;
}

gdk::color webgl1es2_camera::get_clearcolor() const {
    return m_ClearColor;
}

void webgl1es2_camera::activate_clear_mode() const {
    switch(m_ClearMode) {
        case gdk::camera::clear_mode::nothing: return;

        case gdk::camera::clear_mode::color_and_depth: {
            glh::ClearColor(m_ClearColor);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return;
        } 

        case gdk::camera::clear_mode::depth_only: {
            glClear(GL_DEPTH_BUFFER_BIT);
            return;
        } 

        default: break;
    }
    throw graphics_exception("unhandled clear mode");
}

