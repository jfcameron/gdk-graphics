// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/camera.h>
#include <gdk/color.h>
#include <gdk/glh.h>
#include <gdk/intvector2.h>
#include <gdk/mat4x4.h>
#include <gdk/model.h>
#include <gdk/opengl.h>
#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>

using namespace gdk;

camera::camera()
{
    static std::once_flag once;

    std::call_once(once, []()
    {
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_SCISSOR_TEST);
    });
}

void camera::setViewportPosition(const gdk::graphics_vector2_type &a)
{
    m_ViewportPosition = a;
}

void camera::setViewportPosition(const float x, const float y)
{
    m_ViewportPosition.x = x;
    m_ViewportPosition.y = y;
}

void camera::setViewportSize(const gdk::graphics_vector2_type &a)
{
    m_ViewportSize = a;
}
    
void camera::setViewportSize(const float x, const float y)
{
    m_ViewportSize.x = x;
    m_ViewportSize.y = y;
}

void camera::setViewMatrix(const gdk::graphics_vector3_type &aWorldPos, const gdk::graphics_quaternion_type &aRotation)
{
    m_ViewMatrix.setToIdentity();

    m_ViewMatrix.rotate({aRotation.toEuler() * -1});

    m_ViewMatrix.translate(aWorldPos * -1);
}

void camera::setClearcolor(const gdk::color &acolor)
{
    m_Clearcolor = acolor;
}

void camera::setProjection(const graphics_mat4x4_type &matrix)
{
    m_ProjectionMatrix = matrix;
}

void camera::draw(const double aTimeSinceStart, const double aDeltaTime, const gdk::graphics_intvector2_type &aFrameBufferSize, const std::vector<std::shared_ptr<gdk::model>> &amodels) const
{
    gdk::graphics_intvector2_type viewportPixelPosition(aFrameBufferSize * m_ViewportPosition); 

    gdk::graphics_intvector2_type viewportPixelSize (aFrameBufferSize * m_ViewportSize);
    
    glh::Viewport(viewportPixelPosition, viewportPixelSize);

    glh::Scissor(viewportPixelPosition, viewportPixelSize);
    
    switch(m_ClearMode)
    {
        case ClearMode::Color:
        {
            glh::Clearcolor(m_Clearcolor);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } break;
            
        case ClearMode::DepthOnly:
        {
            glClear(GL_DEPTH_BUFFER_BIT);
        } break;
            
        case ClearMode::Nothing: break;
    }

    for (const auto model : amodels) 
        model->draw(aTimeSinceStart, aDeltaTime, m_ViewMatrix, m_ProjectionMatrix);
}

void camera::setProjection(const float aFieldOfView, 
    const float aNearClippingPlane, 
    const float aFarClippingPlane, 
    const float aViewportAspectRatio)
{
    m_ProjectionMatrix.setToPerspective(aFieldOfView, aNearClippingPlane, aFarClippingPlane, aViewportAspectRatio);
}

