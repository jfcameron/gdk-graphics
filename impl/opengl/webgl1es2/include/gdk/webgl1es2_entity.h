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
    /// \detailed Contains a model, a set of uniform collections, a shader, animations, a skeleton.
    ///
    /// \warning I think this class contains a bit too much implementation. (see draw method). It seems weird that "entity"
    /// is responsible for binding and clearing all uniform data for the shader.
    ///
    /// \todo the shaderprogram and uniform data (textures etc.)  Should probably
    /// be broken out into a new abstraction. This work would be a good match for the "material" class seen in many engines.
    ///
    /// \TODO break up all the uniform binding! Simplify!. Model is a dumping ground.. badly defined abstraction, too much responsibility.
    /// TODO replace entity with objects that are more sympathetic to opengl: batch, pipeline, entity.
    class webgl1es2_entity final : public entity
    {
    public: //TODO switch to private
        //! model used when rendering the entity
        std::shared_ptr<webgl1es2_model> m_model;
       
        //! material used when rendering the entity
        std::shared_ptr<webgl1es2_material> m_Material;

        //! Position in the world
        graphics_mat4x4_type m_ModelMatrix;

    public:
        /// \brief draws the webgl1es2_entity at its current world position, with respect to a view and projection matrix.
        /// generally should not be called by the end user. view, proj, are most easily provided to the webgl1es2_entity 
        /// via a camera.
        void draw(const graphics_mat4x4_type &aViewMatrix, const graphics_mat4x4_type &aProjectionMatrix) const;

        /// \brief sets this entity's model.
        void set_model(const std::shared_ptr<webgl1es2_model> a);

        /// \brief sets the model matrix using a vec3 position, quat rotation, vec3 scale
        void set_model_matrix(const graphics_vector3_type &aWorldPos, 
            const graphics_quaternion_type &aRotation, 
            const graphics_vector3_type &aScale = graphics_vector3_type::One);

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
