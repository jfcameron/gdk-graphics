// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_OWNER_H
#define GDK_GFX_OWNER_H

#include <memory>
#include <vector>

namespace gdk { class scene; }

namespace gdk
{
    /// \brief library facing interface for user-defined types that own entities
    ///
    /// This is not required in order to use the library, but if you are extending the
    /// capabilities of this library by e.g: writing a text renderer, animated model etc. having
    /// your types inherit from this interface will allow users of your library to use it
    /// with this library in a more uniform way.
    /// Specifically, they will be able to add and remove your type from a scene in the same way
    /// they add or remove enities from a scene
    class entity_owner
    {
        //! this interface is scene-facing
        friend gdk::scene;

        //! provide access to all of the owner's entities
        virtual std::vector<std::shared_ptr<entity>> get_entities() = 0;

    public:
        virtual ~entity_owner() = default;
    };
}

#endif

