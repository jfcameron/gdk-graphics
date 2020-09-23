// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_ENTITY_H
#define GDK_GFX_WEBGL1ES2_ENTITY_H

#include <gdk/graphics_types.h>
#include <gdk/webgl1es2_texture.h>
#include <gdk/webgl1es2_material.h>
#include <jfc/default_ptr.h>
#include <gdk/entity.h>

#include <iosfwd>
#include <memory>
#include <string_view>

namespace gdk
{
    class webgl1es2_model;
    class webgl1es2_shader_program;
    
    /// \brief Represents an observable 3D object. 
    ///
    /// \detailed Contains a model, a material (shader, uniform data) a shader
	///
	/// TODO: animations, skeletons. These may be more appropriate in a separate animation library
	///
    class webgl1es2_entity final : public entity
    {
    private:
        //! model used when rendering the entity
        std::shared_ptr<webgl1es2_model> m_model;
       
        //! material used when rendering the entity
        std::shared_ptr<webgl1es2_material> m_Material;

        //! Position in the world
        graphics_mat4x4_type m_ModelMatrix;

        //! Whether or not to respect draw calls
        bool m_IsHidden = false;

    public:
        //! do not allow this entity to be drawn
        virtual void hide() override;

        //! allow this entity to be drawn
        virtual void show() override;

        //! check if the entity is hidden
        virtual bool isHidden() const override;

        //! impl
        virtual std::shared_ptr<model> getModel() const override;

        //! impl
        virtual std::shared_ptr<material> getMaterial() const override;

        /// \brief draws the webgl1es2_entity at its current world position, with respect to a view and projection matrix.
        /// generally should not be called by the end user. view, proj, are most easily provided to the webgl1es2_entity 
        /// via a camera.
        //TODO throw if drwa is called and the currently bound model is not m_model?
        void draw(const graphics_mat4x4_type &aViewMatrix, const graphics_mat4x4_type &aProjectionMatrix) const;

        /// \brief sets this entity's model.
		virtual void set_model(const std::shared_ptr<model> a) override;
        //void set_model(const std::shared_ptr<webgl1es2_model> a);

        /// \brief sets the model matrix using a vec3 position, quat rotation, vec3 scale
        virtual void set_model_matrix(const graphics_vector3_type &aWorldPos, 
            const graphics_quaternion_type &aRotation, 
            const graphics_vector3_type &aScale = graphics_vector3_type::One) override;

		virtual void set_model_matrix(const graphics_mat4x4_type& a) override;
		
        /// \brief returns a const ref to the model matrix
        const graphics_mat4x4_type &getModelMatrix() const;

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
    };
}

#endif
