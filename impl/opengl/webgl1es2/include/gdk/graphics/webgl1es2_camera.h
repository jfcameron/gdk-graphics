// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_CAMERA_H
#define GDK_GFX_WEBGL1ES2_CAMERA_H

#include <gdk/graphics/camera.h>
#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/webgl1es2_texture.h>
#include <jfc/unique_handle.h>

#include <memory>
#include <vector>

namespace gdk::graphics
{
    class entity;

    /// \brief webgl1es2_camera implementation of camera
    class webgl1es2_camera
    {
    public:
    /// \name internal interface
    ///@{
    //
        /// \brief position and rotation of the camera in worldspace
        const matrix4x4_type &get_world_matrix() const;

        /// \brief gets the view matrix
        /// \note the view matrix is the inverse of the world matrix
        const matrix4x4_type &get_view_matrix() const;

        /// \brief gets the projection matrix
        const matrix4x4_type &get_projection_matrix() const;

    ///@}
        /// \brief copy semantics
        webgl1es2_camera(const webgl1es2_camera& a) = default;
        /// \brief copy semantics
        webgl1es2_camera& operator=(const webgl1es2_camera & a) = default;

        /// \brief move semantics
        webgl1es2_camera(webgl1es2_camera&&) = default;
        /// \brief move semantics
        webgl1es2_camera& operator=(webgl1es2_camera && a) = default;

        /// \brief constructs with reasonable default values
        webgl1es2_camera();

        virtual ~webgl1es2_camera() = default;

    protected:
    /// \name descendant's interface
    ///@{
    //
        /// \brief build and store an OpenGL perspective projection
        void set_perspective_projection(const float aFieldOfView, const float aNearClippingPlane,
            const float aFarClippingPlane, const float aViewportAspectRatio);

        void set_orthographic_projection(const gdk::graphics::vector2_type &aOrthographicSize,
            const float aNearClippingPlane, const float aFarClippingPlane,
            const float aViewportAspectRatio);

        void set_clear_color(const gdk::graphics::color& acolor);

        void set_clear_mode(const gdk::graphics::camera::clear_mode aClearMode);

        void set_transform(const gdk::graphics::vector3_type& aWorldPos, 
            const gdk::graphics::quaternion_type& aRotation);
        void set_transform(const gdk::graphics::matrix4x4_type &aMatrix);
   
        void activate_clear_mode() const;

        camera::clear_mode get_clearmode() const;

        gdk::graphics::color get_clearcolor() const;
    ///@}

    private:
        matrix4x4_type m_WorldMatrix = matrix4x4_type::identity; 
        matrix4x4_type m_ViewMatrix = matrix4x4_type::identity; 
        matrix4x4_type m_ProjectionMatrix = matrix4x4_type::identity; 

        gdk::graphics::camera::clear_mode m_ClearMode = gdk::graphics::camera::clear_mode::color_and_depth;
        gdk::graphics::color m_ClearColor = color::cornflower_blue;
    };
}

#endif

