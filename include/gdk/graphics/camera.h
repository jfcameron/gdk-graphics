// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_CAMERA_H
#define GDK_GRAPHICS_CAMERA_H

#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>

namespace gdk::graphics {
    /// \brief a point of view a scene is drawn from. A camera starts at the origin looking down -Z, clearing colour
    /// and depth, its clear colour opaque black
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

        /// \brief use a perspective projection whose window is off the view's axis: an asymmetric frustum, as an eye
        /// of a stereo pair sees a screen both eyes converge on, or a panel of a tiled display sees its share
        /// \param aShift how far the window's centre is from the axis, as a share of the window's half extents:
        /// {0, 0} is the symmetric projection above; {0.25, 0} moves the window a quarter of its half width to the
        /// right, so what is on the axis is drawn that far left of the centre
        virtual void set_perspective_projection(const floating_point_type aFieldOfView,
            const floating_point_type aNearClippingPlane, const floating_point_type aFarClippingPlane,
            const floating_point_type aViewportAspectRatio, const vector2_type &aShift) = 0;

        /// \brief use an orthographic projection with the given view volume
        /// `aOrthographicSize` is the full extent of the volume, not its half extents
        virtual void set_orthographic_projection(const gdk::graphics::vector2_type &aOrthographicSize,
            const floating_point_type aNearClippingPlane, const floating_point_type aFarClippingPlane,
            const floating_point_type aViewportAspectRatio) = 0;

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

