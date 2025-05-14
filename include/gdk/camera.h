// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CAMERA_H
#define GDK_GFX_CAMERA_H

#include <gdk/color.h>
#include <gdk/graphics_types.h>

namespace gdk {
    class camera {
    public:
        /// \brief controls which buffers in the active frame buffer object to clear
        enum class clear_mode {
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

        //TODO: consider removing set_ortho, set_perspective. graphics_matrix4x4_type already has methods for those
        // sets projection matrix from an arbitrary 4x4
        //virtual void set_projection(const gdk::graphics_matrix_type &aMatrix) = 0;

        /// \brief checks whether a point in world space is within the camera's frustum 
        //[[nodiscard]] virtual void is_within_frustum(const gdk::graphics_vector3_type &aOtherWorldPosition) = 0;
        //[[nodiscard]] virtual void is_within_frustum(const gdk::graphics_matrix_type &aOtherWorldTransform) = 0;

        /// \brief sets the camera's world and view matrix 
        virtual void set_transform(const gdk::graphics_vector3_type &aWorldPos,
            const gdk::graphics_quaternion_type &aRotation) = 0;
        virtual void set_transform(const gdk::graphics_matrix4x4_type &aMatrix) = 0;

        /// \brief sets the clear color, used to fill color buffer after it is cleared.
        virtual void set_clear_color(const gdk::color &acolor) = 0;

        /// \brief clear mode decides which buffers to clear. see enum
        virtual void set_clear_mode(const clear_mode aClearMode) = 0;

        virtual ~camera() = default;
    };
}

#endif

