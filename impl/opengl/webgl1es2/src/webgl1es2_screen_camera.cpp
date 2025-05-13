// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/webgl1es2_screen_camera.h>
#include <gdk/webgl1es2_texture.h>

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>

using namespace gdk;

webgl1es2_screen_camera::webgl1es2_screen_camera()
: webgl1es2_camera::webgl1es2_camera()
{}

void webgl1es2_screen_camera::set_viewport(const float aX, const float aY, 
    const float aWidth, const float aHeight) {
    m_ViewportPosition.x = aX;
    m_ViewportPosition.y = aY;

    m_ViewportSize.x = aWidth;
    m_ViewportSize.y = aHeight;
}

void webgl1es2_screen_camera::activate(const gdk::graphics_intvector2_type &aFrameBufferSize) const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    decltype(aFrameBufferSize) viewportPixelPosition(aFrameBufferSize.elementwise_product(m_ViewportPosition));
    decltype(aFrameBufferSize) viewportPixelSize(aFrameBufferSize.elementwise_product(m_ViewportSize));
    
    glh::Viewport(viewportPixelPosition, viewportPixelSize);
    glh::Scissor(viewportPixelPosition, viewportPixelSize);
    
    webgl1es2_camera::activate_clear_mode();
}

void webgl1es2_screen_camera::set_perspective_projection(const float aFieldOfView,
    const float aNearClippingPlane, 
    const float aFarClippingPlane, 
    const float aViewportAspectRatio) {
    webgl1es2_camera::set_perspective_projection(aFieldOfView,
        aNearClippingPlane, 
        aFarClippingPlane, 
        aViewportAspectRatio);
}

void webgl1es2_screen_camera::set_orthographic_projection(const float aWidth,
    const float aHeight,
    const float aNearClippingPlane,
    const float aFarClippingPlane,
    const float aViewportAspectRatio) {
    webgl1es2_camera::set_orthographic_projection(aWidth,
        aHeight,
        aNearClippingPlane,
        aFarClippingPlane,
        aViewportAspectRatio);
}

void webgl1es2_screen_camera::set_clear_color(const gdk::color& acolor) {
     webgl1es2_camera::set_clear_color(acolor);
}

void webgl1es2_screen_camera::set_clear_mode(const gdk::camera::clear_mode aClearMode) {
    webgl1es2_camera::set_clear_mode(aClearMode);
}

void webgl1es2_screen_camera::set_transform(const gdk::graphics_vector3_type &aWorldPos, 
    const gdk::graphics_quaternion_type &aRotation) {
    webgl1es2_camera::set_transform(aWorldPos, aRotation);
}

void webgl1es2_screen_camera::set_transform(const gdk::graphics_mat4x4_type &aMatrix) {
    webgl1es2_camera::set_transform(aMatrix);
}

