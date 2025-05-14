// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_ENTITY_H
#define GDK_GFX_WEBGL1ES2_ENTITY_H

#include <gdk/entity.h>
#include <gdk/graphics_types.h>
#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_texture.h>
#include <jfc/default_ptr.h>

#include <iosfwd>
#include <memory>
#include <string_view>

namespace gdk
{
    class webgl1es2_model;
    class webgl1es2_shader_program;
    
    /// \brief Represents an observable 3D object. 
    ///
    /// \detailed Has a position/rotation/scale, a polygonal shape (model), a material (shader, uniforms)
    ///
    class webgl1es2_entity final : public entity {
    public:
    /// \name external interface
    ///@{
    //
        //! do not allow this entity to be drawn
        virtual void hide() override;

        //! allow this entity to be drawn
        virtual void show() override;

        //! check if the entity is hidden
        virtual bool is_hidden() const override;

        /// \brief sets the model matrix using a vec3 position, quat rotation, vec3 scale
        virtual void set_transform(const graphics_vector3_type &aWorldPos, 
            const graphics_quaternion_type &aRotation, 
            const graphics_vector3_type &aScale = graphics_vector3_type::one) override;

        virtual void set_transform(const graphics_matrix4x4_type& a) override;
    ///@}

        //! get the model
        virtual std::shared_ptr<model> getModel() const;

        //! get the material
        virtual std::shared_ptr<material> getMaterial() const;

        /// \brief draws the webgl1es2_entity at its current world position, with respect to a view and projection matrix.
        /// generally should not be called by the end user. view, proj, are most easily provided to the webgl1es2_entity 
        /// via a camera.
        void draw(const graphics_matrix4x4_type &aViewMatrix, const graphics_matrix4x4_type &aProjectionMatrix) const;
		
        /// \brief returns a const ref to the model matrix
        const graphics_matrix4x4_type &getModelMatrix() const;

        /// \brief copy semantics
        webgl1es2_entity(const webgl1es2_entity &) = default;
        /// \brief copy semantics
        webgl1es2_entity &operator=(const webgl1es2_entity &) = default;

        /// \brief move semantics
        webgl1es2_entity(webgl1es2_entity &&) = default;
        /// \brief move semantics
        webgl1es2_entity &operator=(webgl1es2_entity &&) = default;

        //! standard constructor. requires a model and a material
        webgl1es2_entity(const std::shared_ptr<webgl1es2_model>, const std::shared_ptr<webgl1es2_material>);
        
        //! trivial destructor
        ~webgl1es2_entity() = default;
    
    private:
        //! model used when rendering the entity
        std::shared_ptr<webgl1es2_model> m_model;
       
        //! material used when rendering the entity
        std::shared_ptr<webgl1es2_material> m_Material;

        //! Position in the world
        graphics_matrix4x4_type m_ModelMatrix;

        //! Whether or not to respect draw calls
        bool m_IsHidden = false;
    };
}

#endif
