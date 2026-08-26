// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_ENTITY_H
#define GDK_GFX_WEBGL1ES2_ENTITY_H

#include <gdk/graphics/entity.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_texture.h>
#include <jfc/default_ptr.h>

#include <iosfwd>
#include <memory>
#include <string_view>

namespace gdk::graphics
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
        virtual void set_transform(const vector3_type &aWorldPos, 
            const quaternion_type &aRotation, 
            const vector3_type &aScale = vector3_type::one) override;

        virtual void set_transform(const matrix4x4_type& a) override;
    ///@}

        /// \brief this entity's bounding sphere in world space, for culling
        ///
        /// The model's own-space sphere put through the model matrix: the centre is transformed, and
        /// the radius is scaled by the **largest** of the three axis scales -- anything smaller could
        /// report a sphere that does not contain the mesh, and a cull that hides something visible is
        /// a far worse failure than one that draws something it need not have.
        void world_bounds(vector3_type &aCentreOut, floating_point_type &aRadiusOut) const;

        //! get the model
        std::shared_ptr<model> getModel() const;

        //! get the material
        std::shared_ptr<material> getMaterial() const;

        /// \brief draws the webgl1es2_entity at its current world position, with respect to a view and projection matrix.
        /// generally should not be called by the end user. view, proj, are most easily provided to the webgl1es2_entity 
        /// via a camera.
        void draw(const matrix4x4_type &aViewMatrix, const matrix4x4_type &aProjectionMatrix) const;
		
        /// \brief returns a const ref to the model matrix
        const matrix4x4_type &getModelMatrix() const;

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
        matrix4x4_type m_ModelMatrix;

        //! Whether or not to respect draw calls
        bool m_IsHidden = false;
    };
}

#endif
