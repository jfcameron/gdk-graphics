// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SCENE_H
#define GDK_GFX_SCENE_H

#include <gdk/graphics_types.h>

#include <memory>

namespace gdk
{
    class entity;
    class screen_camera;
    class texture_camera;

    /// \brief a 3d environment within which cameras draw entities
    class scene
    {
    public:
        //! add a screen camera to the scene
        virtual void add(const std::shared_ptr<const screen_camera> &pCamera) = 0;
        //! remove a screen camera from the scene
        virtual void remove(const std::shared_ptr<const screen_camera> &pCamera) = 0;
        
        //! add a texture camera to the scene
        virtual void add(const std::shared_ptr<const texture_camera> &pCamera) = 0;
        //! remove a texture camera from the scene
        virtual void remove(const std::shared_ptr<const texture_camera> &pCamera) = 0;
        
        //! add an entity to the scene
        virtual void add(const std::shared_ptr<const entity> &pEntity) = 0;
        //! remove an entity to the scene
        virtual void remove(const std::shared_ptr<const entity> &pEntity) = 0;

        //! draws the scene
        virtual void draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const = 0;

        virtual ~scene() = default;
    };
}

#endif

