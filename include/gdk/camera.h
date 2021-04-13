// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CAMERA_H
#define GDK_GFX_CAMERA_H

#include <gdk/color.h>
#include <gdk/graphics_types.h>

namespace gdk
{
    class camera
    {
    public:
        /// \brief Describes camera clear behaviour: which buffers in the current 
        /// FBO should be cleared?
        enum class clear_mode
        {
            nothing, //!< Do not clear any buffers
            color_and_depth, //!< Clear the color and depth buffers
            depth_only //!< Clear the Depth buffer
        };

        /// \brief sets the projection matrix to a perspective projection
        virtual void set_perspective_projection(const float aFieldOfView,
            const float aNearClippingPlane, 
            const float aFarClippingPlane, 
            const float aViewportAspectRatio) = 0;

        /// \brief sets the projection matrix to an orthographic projection
        virtual void set_orthographic_projection(const float aWidth,
            const float aHeight,
            const float aNearClippingPlane,
            const float aFarClippingPlane,
            const float aViewportAspectRatio) = 0;

        /// \brief rebuilds the view matrix from a 3d position and rotation
        virtual void set_view_matrix(const gdk::graphics_vector3_type &aWorldPos,
            const gdk::graphics_quaternion_type &aRotation) = 0;

        /// \brief sets the clear color, used to fill color buffer after it is cleared.
        virtual void set_clear_color(const gdk::color &acolor) = 0;

        /// \brief clear mode decides which buffers to clear. see enum
        virtual void set_clear_mode(const clear_mode aClearMode) = 0;

        virtual ~camera() = default;
    };
}

#endif

