// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_TEXTURE_CAMERA_H
#define GDK_GRAPHICS_TEXTURE_CAMERA_H

#include <gdk/graphics/camera.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/texture.h>

namespace gdk::graphics {
    /// \brief texture camera renders to textures instead of the screen
    class texture_camera : public camera {
    public:
        //! the texture the camera draws colour into
        [[nodiscard]] virtual texture_ptr_type color_texture() const = 0;

        /// \brief the texture the camera draws depth into
        /// \throws gdk::graphics::exception where the device cannot sample depth
        [[nodiscard]] virtual texture_ptr_type depth_texture() const = 0;

        virtual ~texture_camera() = default;
    };
}

#endif

