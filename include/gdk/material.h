// © Joseph Cameron - All Rights Reserved

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
    /// This includes expected effects, such as texture colors, lightning, 
    /// but can also include unexpected effects, like vertex displacements, transformations.
    /// Material is really pipeline state and persistent uniform values
    class material
    {
    public:
        /// \brief textures can be shared among many webgl1es2_materials.
        using texture_ptr_type = std::shared_ptr<gdk::texture>;

        /// \brief assigns a texture to the material.
        /// \note note that this is passed by pointer, not value
        virtual void setTexture(const std::string &aName, texture_ptr_type aTexturePointer) = 0;

        /// \brief assigns a float to the material.
        virtual void setFloat(const std::string& aName, float aValue) = 0;

        /// \brief assigns a 2 component vector
        virtual void setVector2(const std::string &aName, graphics_vector2_type aValue) = 0;

        /// \brief assigns a 3 component vector
        virtual void setVector3(const std::string &aName, graphics_vector3_type aValue) = 0;
        
        /// \brief assigns a 4 component vector
        virtual void setVector4(const std::string &aName, graphics_vector4_type aValue) = 0;
        
        /// \brief assigns an integer to the material.
        virtual void setInteger(const std::string& aName, int aValue) = 0;
        
        /// \brief assigns a 2 integer array to the material.
        virtual void setInteger2(const std::string& aName, int aValue1, int aValue2) = 0;
        
        /// \brief assigns a 3 integer array to the material.
        virtual void setInteger3(const std::string& aName, int aValue1, int aValue2, 
            int aValue3) = 0;
        
        /// \brief assigns a 4 integer array to the material.
        virtual void setInteger4(const std::string& aName, int aValue1, int aValue2, 
            int aValue3, int aValue4) = 0;

        virtual ~material() = default;
    };
}

#endif

