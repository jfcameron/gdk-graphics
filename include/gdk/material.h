// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MATERIAL_H
#define GDK_GFX_MATERIAL_H

#include <gdk/graphics_types.h>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace gdk {
    class texture;

    /// \brief decides how models using the material should be drawn.
    ///
    /// This includes expected effects, such as texture colors, lightning, 
    /// but can also include unexpected effects, like vertex displacements, transformations.
    /// Material is really pipeline state and persistent uniform values
    //TODO: instead of explicitly naming set_TYPE, do overloads: set_uniform(name, TYPE)
    class material {
    public:
        //! specify whether front- or back-facing polygons can be culled
        enum class face_culling_mode {
            front, //!< cull front facing polygons
            back, //!< cull back facing polygons
            front_and_back, //!< cull front and back facing polygons
            none //!< do not cull any polygons
        };

        //! render mode decides transparency etc
        enum class render_mode {
            opaque, //!< no transparency
            transparent //!< transparency
        };

        /// \brief textures can be shared among many webgl1es2_materials.
        using texture_ptr_type = std::shared_ptr<gdk::texture>;

        /// \brief assigns a texture to the material.
        /// \note note that this is passed by pointer, not value
        virtual void set_texture(const std::string_view aName, texture_ptr_type aTexturePointer) = 0;

        /// \brief assigns a float to the material.
        virtual void set_float(const std::string_view aName, float aValue) = 0;

        /// \brief assigns a 2 component vector
        virtual void set_vector2(const std::string_view aName, graphics_vector2_type aValue) = 0;

        /// \brief assigns a 3 component vector
        virtual void set_vector3(const std::string_view aName, graphics_vector3_type aValue) = 0;
        
        /// \brief assigns a 4 component vector
        virtual void set_vector4(const std::string_view aName, graphics_vector4_type aValue) = 0;
        
        /// \brief assigns an integer to the material.
        virtual void set_integer(const std::string_view aName, int aValue) = 0;
        
        /// \brief assigns a 2 integer array to the material.
        virtual void set_integer2(const std::string_view aName, int aValue1, int aValue2) = 0;
        
        /// \brief assigns a 3 integer array to the material.
        virtual void set_integer3(const std::string_view aName, int aValue1, int aValue2, int aValue3) = 0;
        
        /// \brief assigns a 4 integer array to the material.
        virtual void set_integer4(const std::string_view aName, int aValue1, int aValue2, int aValue3, int aValue4) = 0;

        /// \brief assigns an array of integer vector2s to the material
        virtual void set_int_vector2_array(const std::string_view aName, const std::vector<graphics_intvector2_type> &aValue) = 0;

        virtual ~material() = default;
    };
}

#endif

