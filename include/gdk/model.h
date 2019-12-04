// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    class model
    {
    public:
        virtual ~model() = default;

    protected:
        model() = default;
    };
}

#endif
