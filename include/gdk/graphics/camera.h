// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CAMERA_H
#define GDK_GFX_CAMERA_H

#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>

namespace gdk::graphics {
    class camera {
    public:
        /// \brief controls which buffers in the active frame buffer object to clear
        enum class clear_mode {
            nothing,            //!< Do not clear any buffers
            color_and_depth,    //!< Clear the color and depth buffers
            depth_only          //!< Clear the Depth buffer
        };

        /// \brief use a perspective projection with the given view volume
        /// \param aFieldOfView vertical field of view, in radians
        /// \param aNearClippingPlane distance to the near plane; must be > 0
        /// \param aFarClippingPlane distance to the far plane; must be > aNearClippingPlane
        /// \param aViewportAspectRatio width divided by height
        virtual void set_perspective_projection(const floating_point_type aFieldOfView,
            const floating_point_type aNearClippingPlane, const floating_point_type aFarClippingPlane,
            const floating_point_type aViewportAspectRatio) = 0;

        /// \brief use an orthographic projection with the given view volume
        /// `aOrthographicSize` is the full extent of the volume, not its half extents
        virtual void set_orthographic_projection(const gdk::graphics::vector2_type &aOrthographicSize,
            const floating_point_type aNearClippingPlane, const floating_point_type aFarClippingPlane,
            const floating_point_type aViewportAspectRatio) = 0;
        //virtual void set_projection(gdk::graphics::matrix_type &&aMatrix) = 0;

        /// \brief checks whether a point in world space is within the camera's frustum 
        //[[nodiscard]] virtual void is_within_frustum(const gdk::graphics::vector3_type &aOtherWorldPosition) = 0;
        //[[nodiscard]] virtual void is_within_frustum(const gdk::graphics::matrix_type &aOtherWorldTransform) = 0;

        /// \brief sets the camera's world and view matrix 
        virtual void set_transform(const gdk::graphics::matrix4x4_type &aMatrix) = 0;
        virtual void set_transform(const gdk::graphics::vector3_type &aWorldPos, const gdk::graphics::quaternion_type &aRotation) = 0;

        /// \brief sets the clear color, used to fill color buffer after it is cleared.
        virtual void set_clear_color(const gdk::graphics::color &acolor) = 0;

        /// \brief clear mode decides which buffers to clear. see enum
        virtual void set_clear_mode(const clear_mode aClearMode) = 0;

        virtual ~camera() = default;
    };
}

#endif

