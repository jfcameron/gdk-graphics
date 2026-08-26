// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_TEXTURE_CAMERA_H
#define GDK_GFX_WEBGL1ES2_TEXTURE_CAMERA_H

#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/texture_camera.h>
#include <gdk/graphics/webgl1es2_camera.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <jfc/unique_handle.h>

#include <memory>
#include <optional>
#include <vector>

namespace gdk::graphics {
    class entity;

    /// \brief webgl1es2_camera implementation of texture_camera
    class webgl1es2_texture_camera final : public texture_camera, public webgl1es2_camera {
    public:
    /// \name external interface
    ///@{
    //

        virtual void set_perspective_projection(const float aFieldOfView,
            const float aNearClippingPlane, const float aFarClippingPlane,
            const float aViewportAspectRatio) override;

        virtual void set_orthographic_projection(const gdk::graphics::vector2_type &aOrthographicSize,
            const float aNearClippingPlane, const float aFarClippingPlane,
            const float aViewportAspectRatio) override;

        virtual void set_clear_color(const gdk::graphics::color& acolor) override;

        virtual void set_clear_mode(const clear_mode aClearMode) override;

        virtual void set_transform(const gdk::graphics::vector3_type& aWorldPos, 
            const gdk::graphics::quaternion_type& aRotation) override;
        virtual void set_transform(const gdk::graphics::matrix4x4_type &aMatrix) override;
        
        virtual const std::shared_ptr<gdk::graphics::texture> get_color_texture(const size_t i) const override;

        virtual const std::shared_ptr<gdk::graphics::texture> get_depth_texture() const override;
    ///@}
            
    /// \name internal interface
    ///@{
    //
        void activate() const;
    ///@}
        
        webgl1es2_texture_camera(const intvector2_type &textureSize = {1024, 1024});

        ~webgl1es2_texture_camera() = default;

    private:
        intvector2_type m_TextureSize;

        //! handle to an FBO in the context
        jfc::unique_handle<GLuint> m_FrameBufferHandle;
        
        //! handle to the depth buffer
        jfc::unique_handle<GLuint> m_DepthBuffer;

        //! Texture attached to the color buffer
        mutable std::optional<std::shared_ptr<webgl1es2_texture>> m_ColorBufferTexture;

        //! Texture attached to the depth buffer
        mutable std::optional<std::shared_ptr<webgl1es2_texture>> m_DepthBufferTexture;
    };
}

#endif

