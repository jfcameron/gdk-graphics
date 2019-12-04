// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MATERIAL_H
#define GDK_GFX_MATERIAL_H

namespace gdk
{
    /// \brief decides how models using the material should be drawn
    /// This includes expected effects, such as texture colors, lightning, but can also include unexpected effects, like vertex displacements, transformations.
    /// Material is really pipeline state and persistent uniform values
    class material
    {
        //TODO: implement.
    public:
        virtual ~material() = default;

    protected:
        material() = default;
    };
}

#endif
