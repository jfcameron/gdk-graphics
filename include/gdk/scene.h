// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SCENE_H
#define GDK_GFX_SCENE_H

#include <gdk/graphics_types.h>

namespace gdk
{
    class camera;
    class entity;

    /// \brief a 3d environment within which cameras rasterize entities
    ///
    /// manages api calls to render a scene
    class scene
    {
    public:
        using camera_ptr_type = std::shared_ptr<camera>;
        
        //! entities can be shared across webgl1es2_scenes
        using entity_ptr_type = std::shared_ptr<entity>;

        //! add a camera to the webgl1es2_scene
        virtual void add_camera(camera_ptr_type pCamera) = 0;
        
        virtual void add_entity(entity_ptr_type pEntity) = 0;
        
        //! draws the webgl1es2_scene
        virtual void draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const = 0;
    };
}

#endif
