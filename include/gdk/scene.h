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
        //! cameras can be shared across scenes, therefore shared
        using camera_ptr_type = std::shared_ptr<camera>;
        
        //! entities can be shared across scenes, therefore shared
        using entity_ptr_type = std::shared_ptr<entity>;

    public:
        //! add a camera to the scene
        virtual void add_camera(camera_ptr_type pCamera) = 0;

        //! remove a camera from the scene
        virtual void remove_camera(camera_ptr_type pCamera) = 0;
        
        //! check whether or not this scene contains the camera
        virtual bool contains_camera(camera_ptr_type pCamera) const = 0;
        
        //! add an entity to the scene
        virtual void add_entity(entity_ptr_type pEntity) = 0;
        
        //! remove an entity to the scene
        virtual void remove_entity(entity_ptr_type pEntity) = 0;
        
        //! draws the scene
        virtual void draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const = 0;

        virtual ~scene() = default;
    };
}

#endif
