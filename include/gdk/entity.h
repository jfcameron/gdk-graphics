// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_ENTITY_H
#define GDK_GFX_ENTITY_H

namespace gdk
{
    //! represents an observable 3d object.
    class entity
    {
    public:
        virtual ~entity() = default;

    protected:
        entity() = default;
    };
}

#endif
