// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/camera.h>
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

#include <nlohmann/json.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace gdk;

static constexpr char TAG[] = "Camera";

std::ostream &gdk::operator<<(std::ostream &s, const Camera &a)
{
    return s << nlohmann::json
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },

        {"m_ViewMatrix",        jfc::insertion_operator_to_nlohmann_json_object(a.m_ViewMatrix)},
        {"m_ProjectionMatrix",  jfc::insertion_operator_to_nlohmann_json_object(a.m_ProjectionMatrix)},
        {"m_ViewportPosition",  jfc::insertion_operator_to_nlohmann_json_object(a.m_ViewportPosition)},
        {"m_ViewportSize",      jfc::insertion_operator_to_nlohmann_json_object(a.m_ViewportSize)},
        {"m_FieldOfView",       jfc::insertion_operator_to_nlohmann_json_object(a.m_FieldOfView)},
        {"m_NearClippingPlane", jfc::insertion_operator_to_nlohmann_json_object(a.m_NearClippingPlane)},
        {"m_FarClippingPlane",  jfc::insertion_operator_to_nlohmann_json_object(a.m_FarClippingPlane)},
    }
    .dump();
}

Camera::Camera()
{    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
}

//??????
static inline void calculateOrthographicProjection(gdk::Mat4x4 &aProjectionMatrix, const gdk::Vector2 &aOrthoSize, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio)
{
    (void)aProjectionMatrix;
    (void)aOrthoSize;
    (void)aNearClippingPlane;
    (void)aFarClippingPlane;
    (void)aViewportAspectRatio;
    
    throw std::runtime_error(std::string(TAG).append("Camera::setToOrthographicProjection not implemented!"));
}

//Why does this exist?
static inline void calculatePerspectiveProjection(gdk::Mat4x4& aProjectionMatrix, const float aFieldOfView, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio)
{    
    aProjectionMatrix.setToPerspective(aFieldOfView, aNearClippingPlane, aFarClippingPlane, aViewportAspectRatio);
}

void Camera::draw(const double &aTimeSinceStart, const double &aDeltaTime, const gdk::IntVector2 &aFrameBufferSize, const std::vector<std::shared_ptr<gdk::Model>> &aModels)
{
    gdk::IntVector2 viewportPixelPosition(aFrameBufferSize * m_ViewportPosition);
    gdk::IntVector2 viewportPixelSize    (aFrameBufferSize * m_ViewportSize);
    
    glh::Viewport(viewportPixelPosition, viewportPixelSize);
    glh::Scissor (viewportPixelPosition, viewportPixelSize);
    
    switch(m_ProjectionMode)
    {
        case ProjectionMode::Perspective:
            calculatePerspectiveProjection(m_ProjectionMatrix, m_FieldOfView, m_NearClippingPlane, m_FarClippingPlane, m_ViewportSize.getAspectRatio());
            break;

        default:
            case ProjectionMode::Orthographic:
            calculateOrthographicProjection(m_ProjectionMatrix, m_OrthoSize, m_NearClippingPlane, m_FarClippingPlane, m_ViewportSize.getAspectRatio());
            break;
    }
    
    switch(m_ClearMode)
    {
        case ClearMode::Color:
            glh::ClearColor(m_ClearColor);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;
            
        case ClearMode::DepthOnly:
            glClear(GL_DEPTH_BUFFER_BIT);
            break;
            
        default:
            case ClearMode::Nothing:
            break;
    }

    //
    //
    for (auto model : aModels) model->draw(aTimeSinceStart, aDeltaTime, m_ViewMatrix, getProjectionMatrix());
}

void Camera::setViewMatrix(const gdk::Vector3 &aWorldPos, const gdk::Quaternion &aRotation)
{
    m_ViewMatrix.setToIdentity();
    m_ViewMatrix.rotate({aRotation.toEuler() * -1.f});
    m_ViewMatrix.translate(aWorldPos * -1.f);
}

void Camera::setViewportPosition(const gdk::Vector2 &a)
{
    m_ViewportPosition = a;
}

void Camera::setViewportPosition(const float x, const float y)
{
    m_ViewportPosition.x = x;
    m_ViewportPosition.y = y;
}

gdk::Vector2 Camera::getViewportPosition() const
{
    return m_ViewportPosition;
}

void Camera::setViewportSize(const gdk::Vector2 &a)
{
    m_ViewportSize = a;
}
    
void Camera::setViewportSize(const float x, const float y)
{
    m_ViewportSize.x = x;
    m_ViewportSize.y = y;
}

gdk::Vector2 Camera::getViewportSize() const
{
    return m_ViewportSize;
}

const gdk::Mat4x4 &Camera::getProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

const gdk::Mat4x4 &Camera::getViewMatrix() const
{
    return m_ViewMatrix;
}

void Camera::setClearColor(const gdk::Color &aColor)
{
    m_ClearColor = aColor;
}
