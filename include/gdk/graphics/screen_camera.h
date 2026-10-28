// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_SCREEN_CAMERA_H
#define GDK_GRAPHICS_SCREEN_CAMERA_H

#include <gdk/graphics/camera.h>
#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>

namespace gdk::graphics {
    /// \brief Position, orientation and perspective from which entity(s) are drawn
    class screen_camera : public camera {
    public:
        /// \brief sets the normalized size and position of the viewport within the window
        ///
        /// Viewports that meet share their edge exactly: a pixel is drawn by one of them, never by neither
        virtual void set_viewport(const float aX, 
            const float aY,
            const float aWidth, 
            const float aHeight) = 0;

        /// \brief clip this camera to a rectangle of the window without moving what it draws
        virtual void set_scissor(const float aX,
            const float aY,
            const float aWidth,
            const float aHeight) = 0;

        virtual void clear_scissor() = 0;

        virtual ~screen_camera() = default;
    };
}

#endif

