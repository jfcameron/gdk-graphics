// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/perspective_camera.h>
#include <gdk/color.h>
#include <gdk/glh.h>
#include <gdk/intvector2.h>
#include <gdk/mat4x4.h>
#include <gdk/model.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/opengl.h>
#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <mutex>

#include <nlohmann/json.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace gdk;

static constexpr char TAG[] = "perspective_camera";

std::ostream &gdk::operator<<(std::ostream &s, const perspective_camera &a)
{
    return s << nlohmann::json
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },

        /*{"m_ViewMatrix",        jfc::insertion_operator_to_nlohmann_json_object(a.m_ViewMatrix)},
        {"m_ProjectionMatrix",  jfc::insertion_operator_to_nlohmann_json_object(a.m_ProjectionMatrix)},
        {"m_ViewportPosition",  jfc::insertion_operator_to_nlohmann_json_object(a.m_ViewportPosition)},
        {"m_ViewportSize",      jfc::insertion_operator_to_nlohmann_json_object(a.m_ViewportSize)},
        {"m_FieldOfView",       jfc::insertion_operator_to_nlohmann_json_object(a.m_FieldOfView)},
        {"m_NearClippingPlane", jfc::insertion_operator_to_nlohmann_json_object(a.m_NearClippingPlane)},
        {"m_FarClippingPlane",  jfc::insertion_operator_to_nlohmann_json_object(a.m_FarClippingPlane)},*/
    }
    .dump();
}


perspective_camera::perspective_camera()
{
    static std::once_flag once;

    std::call_once(once, []()
    {
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_SCISSOR_TEST);
    });
}

//??????
static inline void calculateOrthographicProjection(gdk::graphics_mat4x4_type &aProjectionMatrix, const gdk::graphics_vector2_type &aOrthoSize, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio)
{
    (void)aProjectionMatrix;
    (void)aOrthoSize;
    (void)aNearClippingPlane;
    (void)aFarClippingPlane;
    (void)aViewportAspectRatio;
    
    throw std::runtime_error(std::string(TAG).append("perspective_camera::setToOrthographicProjection not implemented!"));
}

//Why does this exist?
static inline void calculatePerspectiveProjection(gdk::graphics_mat4x4_type &aProjectionMatrix, const float aFieldOfView, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio)
{    
    aProjectionMatrix.setToPerspective(aFieldOfView, aNearClippingPlane, aFarClippingPlane, aViewportAspectRatio);
}

void perspective_camera::draw(const double &aTimeSinceStart, const double &aDeltaTime, const gdk::graphics_intvector2_type &aFrameBufferSize, const std::vector<std::shared_ptr<gdk::Model>> &aModels)
{
    gdk::graphics_intvector2_type viewportPixelPosition(aFrameBufferSize * m_ViewportPosition); 
    gdk::graphics_intvector2_type viewportPixelSize    (aFrameBufferSize * m_ViewportSize);
    
    glh::Viewport(viewportPixelPosition, viewportPixelSize);
    glh::Scissor (viewportPixelPosition, viewportPixelSize);
    
    switch(m_ProjectionMode)
    {
        case ProjectionMode::Perspective:
        {
            calculatePerspectiveProjection(m_ProjectionMatrix, m_FieldOfView, m_NearClippingPlane, m_FarClippingPlane, m_ViewportSize.getAspectRatio());
        } break;

        case ProjectionMode::Orthographic:
        {
            calculateOrthographicProjection(m_ProjectionMatrix, m_OrthoSize, m_NearClippingPlane, m_FarClippingPlane, m_ViewportSize.getAspectRatio());
        } break;
    }
    
    switch(m_ClearMode)
    {
        case ClearMode::Color:
        {
            glh::ClearColor(m_ClearColor);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } break;
            
        case ClearMode::DepthOnly:
        {
            glClear(GL_DEPTH_BUFFER_BIT);
        } break;
            
        case ClearMode::Nothing: break;
    }

    for (auto model : aModels) model->draw(aTimeSinceStart, aDeltaTime, m_ViewMatrix, getProjectionMatrix());
}

void perspective_camera::setViewMatrix(const gdk::graphics_vector3_type &aWorldPos, const gdk::graphics_quaternion_type &aRotation)
{
    m_ViewMatrix.setToIdentity();
    m_ViewMatrix.rotate({aRotation.toEuler() * -1});
    m_ViewMatrix.translate(aWorldPos * -1);
}

void perspective_camera::setViewportPosition(const gdk::graphics_vector2_type &a)
{
    m_ViewportPosition = a;
}

void perspective_camera::setViewportPosition(const float x, const float y)
{
    m_ViewportPosition.x = x;
    m_ViewportPosition.y = y;
}

gdk::graphics_vector2_type perspective_camera::getViewportPosition() const
{
    return m_ViewportPosition;
}

void perspective_camera::setViewportSize(const gdk::graphics_vector2_type &a)
{
    m_ViewportSize = a;
}
    
void perspective_camera::setViewportSize(const float x, const float y)
{
    m_ViewportSize.x = x;
    m_ViewportSize.y = y;
}

gdk::graphics_vector2_type perspective_camera::getViewportSize() const
{
    return m_ViewportSize;
}

const gdk::graphics_mat4x4_type &perspective_camera::getProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

const gdk::graphics_mat4x4_type &perspective_camera::getViewMatrix() const
{
    return m_ViewMatrix;
}

void perspective_camera::setClearColor(const gdk::Color &aColor)
{
    m_ClearColor = aColor;
}

