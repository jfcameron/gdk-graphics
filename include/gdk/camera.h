// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CAMERA_H
#define GDK_GFX_CAMERA_H

#include <gdk/graphics_types.h>
#include <gdk/color.h>

#include <memory>
#include <vector>

namespace gdk
{
    class model;

    /// \brief Position, orientation and perspective from which model(s) are drawn
    class camera
    {
        //friend std::ostream &operator<< (std::ostream &, const perspective_camera &);

        /// \brief Describes camera clear behaviour: which buffers in the current FBO should be cleared?
        enum class ClearMode 
        {
            Nothing,  //!< Do not clear any buffers
            Color,    //!< Clear the color buffer
            DepthOnly //!< Clear the Depth buffer
        };                          

    private:
        /// \brief position of the camera viewport within the device viewport
        graphics_vector2_type m_ViewportPosition = graphics_vector2_type::Zero;

        /// \brief size of camera viewport within the device viewport
        graphics_vector2_type m_ViewportSize = graphics_vector2_type(1., 1.); 
        
        /// \brief World position of camera
        graphics_mat4x4_type m_ViewMatrix = graphics_mat4x4_type::Identity; 

        /// \brief Projection of the camera
        graphics_mat4x4_type m_ProjectionMatrix = graphics_mat4x4_type::Identity; 

        /// \brief Determines which buffers in the FBO to clear before drawing
        ClearMode m_ClearMode = ClearMode::Color;

        /// \brief The color to replace all data in the color buffer with 
        /// (if color buffer is to be cleared)
        gdk::color m_Clearcolor = color::CornflowerBlue;

        //TODO: support render texture
        //Rendertexture m_Rendertexture;

    public:
        /// \brief set clear color
        void setClearcolor(const gdk::color &acolor);

        /// \brief sets the top left position of the viewport within the screen
        void setViewportPosition(const graphics_vector2_type &a);
        /// \brief override
        void setViewportPosition(const float x, const float y);

        /// \brief set size of viewport in surface
        void setViewportSize(const graphics_vector2_type &a);
        /// \brief override
        void setViewportSize(const float x, const float y);

        /// \brief rebuilds the view matrix from a 3d position and rotation
        void setViewMatrix(const gdk::graphics_vector3_type &aWorldPos, const gdk::graphics_quaternion_type &aRotation);

        /// \brief sets projection matrix via copy
        void setProjection(const graphics_mat4x4_type &matrix);

        /// \brief sets the projection matrix to a perspective projection //TODO: change name? is overloading really appropriate?
        void setProjection(const float aFieldOfView, 
            const float aNearClippingPlane, 
            const float aFarClippingPlane, 
            const float aViewportAspectRatio);

        /// \brief set projection matrix from orthographic bounds
        //void setProject(height, width, depth);

        /// \brief Draws a list of models to the framebuffer
        void draw(const double aTimeSinceStart, const double aDeltaTime, const gdk::graphics_intvector2_type &aFrameBufferSize, const std::vector<std::shared_ptr<gdk::model>> &amodels) const;

        /// \brief copy semantics
        camera(const camera &a) = default;
        /// \brief copy semantics
        camera &operator=(const camera &a) = default;
       
        /// \brief move semantics
        camera(camera &&) = default;
        /// \brief move semantics
        camera &operator=(camera &&a) = default;

        /// \brief constructs a camera with reasonable default values
        camera();

        ~camera() = default;
    };
}

#endif
