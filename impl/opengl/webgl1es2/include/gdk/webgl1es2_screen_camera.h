// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_SCREEN_CAMERA_H
#define GDK_GFX_WEBGL1ES2_SCREEN_CAMERA_H

#include <gdk/color.h>
#include <gdk/graphics_types.h>
#include <gdk/screen_camera.h>
#include <gdk/webgl1es2_camera.h>
#include <gdk/webgl1es2_texture.h>
#include <jfc/unique_handle.h>

#include <memory>
#include <vector>

namespace gdk {
    class entity;

    /// \brief webgl1es2_screen_camera implementation of camera
    class webgl1es2_screen_camera final : public screen_camera, public webgl1es2_camera {
    public:
    /// \name external interface
    ///@{
    //
        /// \brief sets the normalized size and position of the viewport within of the window
        virtual void set_viewport(const float aX, 
                const float aY,
                const float aWidth, 
                const float aHeight) override;

        /// \brief sets the projection matrix to a perspective projection
        virtual void set_perspective_projection(const float aFieldOfView,
            const float aNearClippingPlane, 
            const float aFarClippingPlane, 
            const float aViewportAspectRatio) override;

        /// \brief sets the projection matrix to an orthographic projection
        virtual void set_orthographic_projection(const float aWidth,
                const float aHeight,
                const float aNearClippingPlane,
                const float aFarClippingPlane,
                const float aViewportAspectRatio) override;

        /// \brief rebuilds the view matrix from a 3d position and rotation
        virtual void set_transform(const gdk::graphics_vector3_type &aWorldPos,
            const gdk::graphics_quaternion_type &aRotation) override;
        virtual void set_transform(const gdk::graphics_mat4x4_type &aMatrix) override;

        /// \brief sets the clear color, used to fill color buffer after it is cleared.
        virtual void set_clear_color(const gdk::color &acolor) override;

        /// \brief clear mode decides which buffers to clear. see enum
        virtual void set_clear_mode(const gdk::camera::clear_mode aClearMode) override;
    ///@}

    /// \name internal interface
    ///@{
    //
        /// \brief activates this camera; mutates gl according to this camera's state
        void activate(const gdk::graphics_intvector2_type& aFrameBufferSize) const;
    ///@}

        /// \brief copy semantics
        webgl1es2_screen_camera(const webgl1es2_screen_camera& a) = default;
        /// \brief copy semantics
        webgl1es2_screen_camera& operator=(const webgl1es2_screen_camera & a) = default;

        /// \brief move semantics
        webgl1es2_screen_camera(webgl1es2_screen_camera&&) = default;
        /// \brief move semantics
        webgl1es2_screen_camera& operator=(webgl1es2_screen_camera && a) = default;

        /// \brief constructs with reasonable default values
        webgl1es2_screen_camera();

        virtual ~webgl1es2_screen_camera() = default;

    private:
        graphics_vector2_type m_ViewportPosition = graphics_vector2_type::Zero;
        graphics_vector2_type m_ViewportSize = graphics_vector2_type(1, 1); 
    };
}

#endif

