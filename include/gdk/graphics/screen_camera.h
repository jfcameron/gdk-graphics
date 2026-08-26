// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SCREEN_CAMERA_H
#define GDK_GFX_SCREEN_CAMERA_H

#include <gdk/graphics/camera.h>
#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>

namespace gdk::graphics {
    /// \brief Position, orientation and perspective from which entity(s) are drawn
    /// \todo cull functor
    /// \todo project vector from viewport coord into world
    class screen_camera : public camera {
    public:
        /// \brief sets the normalized size and position of the viewport within of the window
        virtual void set_viewport(const float aX, 
            const float aY,
            const float aWidth, 
            const float aHeight) = 0;

        virtual ~screen_camera() = default;
    };
}

#endif

