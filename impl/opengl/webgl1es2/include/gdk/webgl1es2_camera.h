// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_CAMERA_H
#define GDK_GFX_WEBGL1ES2_CAMERA_H

#include <gdk/camera.h>
#include <gdk/color.h>
#include <gdk/graphics_types.h>
#include <gdk/webgl1es2_texture.h>
#include <jfc/unique_handle.h>

#include <memory>
#include <vector>

namespace gdk
{
    class entity;

    /// \brief webgl1es2_camera implementation of camera
    class webgl1es2_camera
    {
    public:
    /// \name internal interface
    ///@{
    //
        /// \brief position and rotation of the camera in worldspace, same as an entity model matrix
        graphics_mat4x4_type get_world_matrix() const;

        /// \brief gets the view matrix
        /// \note the view matrix is the inverse of the world matrix
        graphics_mat4x4_type get_view_matrix() const;

        /// \brief gets the projection matrix
        graphics_mat4x4_type get_projection_matrix() const;

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
        void set_perspective_projection(const float aFieldOfView,
            const float aNearClippingPlane,
            const float aFarClippingPlane,
            const float aViewportAspectRatio);

        void set_orthographic_projection(const float aWidth,
            const float aHeight,
            const float aNearClippingPlane,
            const float aFarClippingPlane,
            const float aViewportAspectRatio);

        void set_clear_color(const gdk::color& acolor);

        void set_clear_mode(const gdk::camera::clear_mode aClearMode);

        void set_transform(const gdk::graphics_vector3_type& aWorldPos, 
            const gdk::graphics_quaternion_type& aRotation);
        void set_transform(const gdk::graphics_mat4x4_type &aMatrix);
   
        void activate_clear_mode() const;

        camera::clear_mode get_clearmode() const;

        gdk::color get_clearcolor() const;
    ///@}

    private:
        graphics_mat4x4_type m_WorldMatrix = graphics_mat4x4_type::identity; 
        graphics_mat4x4_type m_ViewMatrix = graphics_mat4x4_type::identity; 
        graphics_mat4x4_type m_ProjectionMatrix = graphics_mat4x4_type::identity; 

        gdk::camera::clear_mode m_ClearMode = gdk::camera::clear_mode::color_and_depth;
        gdk::color m_ClearColor = color::cornflower_blue;
    };
}

#endif

