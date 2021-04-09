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
    class webgl1es2_camera : public camera
    {
    public:
    /// \name external interface
    ///@{
    //
        virtual void set_perspective_projection(const float aFieldOfView,
            const float aNearClippingPlane,
            const float aFarClippingPlane,
            const float aViewportAspectRatio) override;

        virtual void set_orthographic_projection(const float aWidth,
            const float aHeight,
            const float aNearClippingPlane,
            const float aFarClippingPlane,
            const float aViewportAspectRatio) override;

        virtual void set_viewport(const float aX,
            const float aY,
            const float aWidth,
            const float aHeight) override;

        virtual void set_clear_color(const gdk::color& acolor) override;

        virtual void set_clear_mode(const clear_mode aClearMode) override;

        virtual void set_view_matrix(const gdk::graphics_vector3_type& aWorldPos, 
            const gdk::graphics_quaternion_type& aRotation) override;

    ///@}

    /// \name internal interface
    ///@{
    //
        /// \brief activates this camera; mutates gl according to this camera's state
        void activate(const gdk::graphics_intvector2_type& aFrameBufferSize) const;

        /// \brief gets the view matrix
        graphics_mat4x4_type get_view_matrix() const;

        /// \brief gets the projection matrix
        graphics_mat4x4_type get_projection_matrix() const;

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
    ///@}

    private:
        graphics_vector2_type m_ViewportPosition = graphics_vector2_type::Zero;
        graphics_vector2_type m_ViewportSize = graphics_vector2_type(1, 1); 
        
        graphics_mat4x4_type m_ViewMatrix = graphics_mat4x4_type::Identity; 
        graphics_mat4x4_type m_ProjectionMatrix = graphics_mat4x4_type::Identity; 

        clear_mode m_ClearMode = clear_mode::color_and_depth;
        gdk::color m_ClearColor = color::CornflowerBlue;
    };
}

#endif
