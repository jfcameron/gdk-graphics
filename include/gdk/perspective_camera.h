// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_PERSPECTIVE_CAMERA_H
#define GDK_GFX_PERSPECTIVE_CAMERA_H

#include <gdk/graphics_types.h>
#include <gdk/color.h>
#include <gdk/camera.h>

#include <iosfwd>
#include <memory>
#include <vector>

namespace gdk
{
    class Model;
    
    /// \brief Position, orientation and perspective from which Model(s) are drawn
    /// \TODO separate ortho and persepective into subclasses. perspective_camera is modal. big no no.
    class perspective_camera final : public camera
    {
        friend std::ostream &operator<< (std::ostream &, const perspective_camera &);
            
    public:        
        enum class ProjectionMode {Perspective, Orthographic}; //!< The camera's projection mode
            
    private:
        graphics_mat4x4_type m_ViewMatrix =       graphics_mat4x4_type::Identity; //!< World position of camera
        graphics_mat4x4_type m_ProjectionMatrix = graphics_mat4x4_type::Identity; //!< Projection of the camera
            
        ClearMode  m_ClearMode =  ClearMode::Color;      //!< Determines which buffers in the FBO to clear before drawing
        gdk::Color m_ClearColor = Color::CornflowerBlue; //!< The color to replace all data in the color buffer with (if color buffer is to be cleared)
            
        graphics_vector2_type m_ViewportPosition = graphics_vector2_type::Zero;   //!< position of the camera viewport within the device viewport
        graphics_vector2_type m_ViewportSize =     graphics_vector2_type(1., 1.); //!< size of camera viewport within the device viewport

        //This is a bit logically messy. I dont know if this approach for generating the projection matrix is as legible as possible. This is a modal class. Shouldnt this be two subclasses?
        ProjectionMode m_ProjectionMode =    ProjectionMode::Perspective; //this will be removed when subclassing
        
        graphics_vector2_type m_OrthoSize = {10, 10}; // this belongs to ortho
        float m_FieldOfView = {90.}; // this belongs to perspective

        float m_NearClippingPlane = {0.001};
        float m_FarClippingPlane =  {20.};
            
        //RenderTexture m_RenderTexture;
            
    public:
        void setViewportPosition(const graphics_vector2_type &);
        void setViewportPosition(const float x, const float y);
        graphics_vector2_type getViewportPosition() const;
            
        void setViewportSize(const graphics_vector2_type &);
        void setViewportSize(const float x, const float y);
        graphics_vector2_type getViewportSize() const;
            
        void setToOrthographicProjection(const graphics_vector2_type &aOrthoSize, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio);            
        void setToPerspectiveProjection(const float aFieldOfView, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio);
        const graphics_mat4x4_type &getProjectionMatrix() const;
             
        void setViewMatrix(const gdk::graphics_vector3_type &aWorldPos, const gdk::graphics_quaternion_type &aRotation);
        const graphics_mat4x4_type &getViewMatrix() const;

        void setClearColor(const gdk::Color &aColor);
            
        //! Draws a list of models to the framebuffer
        void draw(const double &aTimeSinceStart, const double &aDeltaTime, const gdk::graphics_intvector2_type &aFrameBufferSize, const std::vector<std::shared_ptr<gdk::Model>> &aModels);
            
        perspective_camera& operator=(const perspective_camera &) = delete;
        perspective_camera& operator=(perspective_camera &&) = delete;
      
        perspective_camera();
        perspective_camera(const perspective_camera &) = delete;
        perspective_camera(perspective_camera &&) = default;
        ~perspective_camera() = default;
    };

    std::ostream &operator<< (std::ostream &, const perspective_camera &);
}

#endif
