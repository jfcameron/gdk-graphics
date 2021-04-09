// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_CAMERA_H
#define GDK_GFX_TEXTURE_CAMERA_H

#include <gdk/camera.h>
#include <gdk/graphics_types.h>

namespace gdk
{
    /// \brief texture camera renders to textures instead of the screen
    class texture_camera : public camera
    {
    public:
        //virtual get_texture blah blah const

        virtual ~texture_camera() = default;
    };
}

#endif

