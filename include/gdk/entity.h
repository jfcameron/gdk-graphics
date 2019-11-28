// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <gdk/graphics_types.h>
#include <gdk/texture.h>
#include <jfc/default_ptr.h>

#include <iosfwd>
#include <memory>
#include <string_view>

namespace gdk
{
    class vertex_data;
    class shader_program;
    
    /// \brief Represents an observable 3D object. 
    ///
    /// \detailed Contains a vertex_data, a set of uniform collections, a shader, animations, a skeleton.
    ///
    /// \warning I think this class contains a bit too much implementation. (see draw method). It seems weird that "entity"
    /// is responsible for binding and clearing all uniform data for the shader.
    ///
    /// \todo the shaderprogram and uniform data (textures etc.)  Should probably
    /// be broken out into a new abstraction. This work would be a good match for the "material" class seen in many engines.
    ///
    /// \TODO break up all the uniform binding! Simplify!. Model is a dumping ground.. badly defined abstraction, too much responsibility.
    /// TODO replace entity with objects that are more sympathetic to opengl: batch, pipeline, entity.
    class entity final
    {
        //TODO refactor to batch
        std::shared_ptr<vertex_data> m_vertex_data;
        
        //TODO refactor to pipeline
        std::shared_ptr<shader_program> m_ShaderProgram;

        //! Position in the world
        graphics_mat4x4_type m_ModelMatrix;

    public:
        //TODO: factor this out. Entities bind but do not draw
        /// \brief draws the entity at its current world position, with respect to a view and projection matrix.
        /// generally should not be called by the end user. view, proj, are most easily provided to the entity via a camera.
        void draw(
            const double aTimeSinceStart, //TODO: remove
            const double aDeltaTime, //Could be useful for calculating delta velocity and acceleration for interpolation

            const graphics_mat4x4_type &aViewMatrix,      //OK: neeed for mvp
            const graphics_mat4x4_type &aProjectionMatrix //OK: Needed in order to calculate MVP
            ) const;

        void set_vertex_data(const std::shared_ptr<vertex_data> a);
            
        void set_model_matrix(const graphics_vector3_type &aWorldPos, 
            const graphics_quaternion_type &aRotation, 
            const graphics_vector3_type &aScale = graphics_vector3_type::One);

        const graphics_mat4x4_type &getModelMatrix() const;

        /// \brief copy semantics
        entity(const entity &) = default;
        /// \brief copy semantics
        entity &operator=(const entity &) = default;

        /// \brief move semantics
        entity(entity &&) = default;
        /// \brief move semantics
        entity &operator=(entity &&) = default;

        entity(const std::shared_ptr<vertex_data>, const std::shared_ptr<shader_program>);

        ~entity() = default;
    };
}

#endif
