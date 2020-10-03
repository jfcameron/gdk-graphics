// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MATERIAL_H
#define GDK_GFX_MATERIAL_H

#include <string>
#include <memory>

#include <gdk/graphics_types.h>

namespace gdk
{
    class texture;

    /// \brief decides how models using the material should be drawn.
    ///
    /// This includes expected effects, such as texture colors, lightning, but can also include unexpected effects, like vertex displacements, transformations.
    /// Material is really pipeline state and persistent uniform values
    class material
    {
    public:
        /// \brief textures can be shared among many webgl1es2_materials.
        using texture_ptr_type = std::shared_ptr<gdk::texture>;

		/// \brief assigns a float to the material.
		virtual void element::setFloat(const std::string& aTextureName, float aTexture) = 0;

        /// \brief assigns a texture to the material.
        virtual void element::setTexture(const std::string &aTextureName, texture_ptr_type aTexture) = 0;

		/// \brief assigns a vector2
		virtual void setVector2(const std::string &aVector2Name, graphics_vector2_type aVector2) = 0;

        virtual ~material() = default;

    protected:
        material() = default;
    };
}

#endif
