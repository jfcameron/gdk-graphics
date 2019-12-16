// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MATERIAL_H
#define GDK_GFX_MATERIAL_H

#include <string>
#include <memory>

namespace gdk
{
    class texture;

    /// \brief decides how models using the material should be drawn
    /// This includes expected effects, such as texture colors, lightning, but can also include unexpected effects, like vertex displacements, transformations.
    /// Material is really pipeline state and persistent uniform values
    class material
    {
    public:
        //! textures can be shared among many webgl1es2_materials.
        using texture_ptr_type = std::shared_ptr<gdk::texture>;

        //! assigns a texture to the material.
        virtual void setTexture(const std::string &aTextureName, texture_ptr_type aTexture) = 0;

        virtual ~material() = default; //!< dtor

    protected:
        material() = default; //!< interface type cannot be directly instantiated
    };
}

#endif
