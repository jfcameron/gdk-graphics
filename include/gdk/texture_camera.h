// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_CAMERA_H
#define GDK_GFX_TEXTURE_CAMERA_H

#include <gdk/camera.h>
#include <gdk/graphics_types.h>
#include <gdk/texture.h>

namespace gdk {
    /// \brief texture camera renders to textures instead of the screen
    class texture_camera : public camera {
    public:
        //! get a texture attached to the color buffer
        virtual const std::shared_ptr<gdk::texture> get_color_texture(const size_t i = 0) const = 0;

        //! get the texture attached to the depth buffer
        virtual const std::shared_ptr<gdk::texture> get_depth_texture() const = 0;
        
        virtual ~texture_camera() = default;
    };
}

#endif

