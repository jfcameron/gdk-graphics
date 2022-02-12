// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_ENTITY_H
#define GDK_GFX_ENTITY_H

#include <gdk/graphics_types.h>

namespace gdk
{
    class material;
    class model;

    //! represents an observable 3d object.
    class entity
    {
    public:
        //! do not allow this entity to be drawn
        virtual void hide() = 0;

        //! allow this entity to be drawn
        virtual void show() = 0;

        /// \brief check if the entity is hidden
        virtual bool isHidden() const = 0;
        
        /// \brief sets the model matrix using a vec3 position, quat rotation, vec3 scale
        virtual void set_model_matrix(const graphics_vector3_type &aWorldPos, 
            const graphics_quaternion_type &aRotation, 
            const graphics_vector3_type &aScale = graphics_vector3_type::One) = 0;

        /// \brief set the model matrix using a matrix
        virtual void set_model_matrix(const graphics_mat4x4_type& a) = 0;

        //! dtor
        virtual ~entity() = default;
    };
}

#endif

