// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SCENE_H
#define GDK_GFX_SCENE_H

#include <gdk/graphics_types.h>

#include <memory>

namespace gdk
{
    class entity;
    class entity_owner;
    class screen_camera;
    class texture_camera;

    /// \brief a 3d environment within which cameras draw entities
    class scene
    {
    public:
        //! entities can be shared across scenes, therefore shared
        using entity_ptr_type = std::shared_ptr<entity>;

    public:
        //! add an owner to the scene
        void add(std::shared_ptr<entity_owner> pOwner);
        //! add a screen camera to the scene
        virtual void add(std::shared_ptr<screen_camera> pCamera) = 0;
        //! add a texture camera to the scene
        virtual void add(std::shared_ptr<texture_camera> pCamera) = 0;
        //! add an entity to the scene
        virtual void add(entity_ptr_type pEntity) = 0;

        //! remove an owner
        void remove(std::shared_ptr<entity_owner> pOwner);
        //! remove a screen camera from the scene
        virtual void remove(std::shared_ptr<screen_camera> pCamera) = 0;
        //! remove a texture camera from the scene
        virtual void remove(std::shared_ptr<texture_camera> pCamera) = 0;
        //! remove an entity to the scene
        virtual void remove(entity_ptr_type pEntity) = 0;

        //! draws the scene
        virtual void draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const = 0;

        virtual ~scene() = default;
    };
}

#endif

