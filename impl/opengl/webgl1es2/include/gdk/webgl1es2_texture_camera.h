// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_TEXTURE_CAMERA_H
#define GDK_GFX_WEBGL1ES2_TEXTURE_CAMERA_H

#include <gdk/color.h>
#include <gdk/graphics_types.h>
#include <gdk/texture_camera.h>
#include <gdk/webgl1es2_camera.h>
#include <gdk/webgl1es2_texture.h>

#include <jfc/unique_handle.h>

#include <memory>
#include <vector>

namespace gdk
{
    class entity;

    /// \brief webgl1es2_camera implementation of texture_camera
    class webgl1es2_texture_camera final : public texture_camera, public webgl1es2_camera
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

    webgl1es2_texture_camera();

    ~webgl1es2_texture_camera() = default;

    private:
        //! handle to an FBO in the context
        jfc::unique_handle<GLuint> m_FrameBufferHandle;        
        //! handle to the texture attached to the FBO's color buffer 
        jfc::unique_handle<GLuint> m_ColorBufferTextureHandle;
        //TODO: depth.......etc.
    };
}

#endif

