// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/webgl1es2_camera.h>
#include <gdk/webgl1es2_texture.h>

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>

using namespace gdk;

webgl1es2_camera::webgl1es2_camera()
{
    static std::once_flag once;

    std::call_once(once, []()
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
    });
}

void webgl1es2_camera::set_viewport(const float aX, const float aY, 
    const float aWidth, const float aHeight)
{
    m_ViewportPosition.x = aX;
    m_ViewportPosition.y = aY;

    m_ViewportSize.x = aWidth;
    m_ViewportSize.y = aHeight;
}

void webgl1es2_camera::set_clear_color(const gdk::color& acolor)
{
	m_ClearColor = acolor;
}

void webgl1es2_camera::set_clear_mode(const clear_mode aClearMode)
{
    m_ClearMode = aClearMode;
}

void webgl1es2_camera::set_view_matrix(const gdk::graphics_vector3_type &aWorldPos, const gdk::graphics_quaternion_type &aRotation)
{
    m_ViewMatrix.setToIdentity();

    m_ViewMatrix.rotate({aRotation.toEuler() * -1});

    m_ViewMatrix.translate(aWorldPos * -1);
}

void webgl1es2_camera::activate(const gdk::graphics_intvector2_type &aFrameBufferSize) const
{
    decltype(aFrameBufferSize) viewportPixelPosition(aFrameBufferSize * m_ViewportPosition);
    decltype(aFrameBufferSize) viewportPixelSize(aFrameBufferSize * m_ViewportSize);
    
    glh::Viewport(viewportPixelPosition, viewportPixelSize);
    glh::Scissor(viewportPixelPosition, viewportPixelSize);
    
    switch(m_ClearMode)
    {
        case clear_mode::color_and_depth:
        {
            glh::Clearcolor(m_ClearColor);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } break;
            
        case clear_mode::depth_only:
        {
            glClear(GL_DEPTH_BUFFER_BIT);
        } break;
            
        case clear_mode::nothing: break;
    }
}

void webgl1es2_camera::set_perspective_projection(const float aFieldOfView,
    const float aNearClippingPlane, 
    const float aFarClippingPlane, 
    const float aViewportAspectRatio)
{
    m_ProjectionMatrix.setToPerspective(aFieldOfView, aNearClippingPlane, aFarClippingPlane, aViewportAspectRatio);
}

void webgl1es2_camera::set_orthographic_projection(const float aWidth,
    const float aHeight,
    const float aNearClippingPlane,
    const float aFarClippingPlane,
    const float aViewportAspectRatio)
{
    m_ProjectionMatrix.setToOrthographic({ aWidth, aHeight }, aNearClippingPlane, aFarClippingPlane, aViewportAspectRatio);
}

graphics_mat4x4_type webgl1es2_camera::get_view_matrix() const 
{ 
    return m_ViewMatrix; 
}

graphics_mat4x4_type webgl1es2_camera::get_projection_matrix() const 
{ 
    return m_ProjectionMatrix; 
}

