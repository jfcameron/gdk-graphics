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
    /// \warning I think this class contains a bit too much implementation. (see draw method). It seems weird that "model"
    /// is responsible for binding and clearing all uniform data for the shader.
    ///
    /// \todo the shaderprogram and uniform data (textures etc.)  Should probably
    /// be broken out into a new abstraction. This work would be a good match for the "material" class seen in many engines.
    ///
    /// \TODO break up all the uniform binding! Simplify!. Model is a dumping ground.. badly defined abstraction, too much responsibility.
    /// TODO replace model with objects that are more sympathetic to opengl: batch, pipeline, entity.
    class model final
    {
        //! Position in the world
        graphics_mat4x4_type m_modelMatrix;

        //TODO refactor to batch
        std::shared_ptr<vertex_data> m_vertex_data;

        //TODO refactor to pipeline
        std::shared_ptr<shader_program> m_ShaderProgram;

    public:
        /// \brief draws the model at its current world position, with respect to a view and projection matrix.
        /// generally should not be called by the end user. view, proj, are most easily provided to the model via a camera.
        void draw(
            //const shader_program &aCurrentShaderProgram // Required to assign uniforms 

            const double aTimeSinceStart, //TODO: remove
            const double aDeltaTime, //Could be useful for calculating delta velocity and acceleration for interpolation

            const graphics_mat4x4_type &aViewMatrix,      //OK: neeed for mvp
            const graphics_mat4x4_type &aProjectionMatrix //OK: Needed in order to calculate MVP
            ) const;

        void setvertex_data(const std::shared_ptr<vertex_data> a);

        // TODO: need shader_ptr!
        void set_texture(const std::string &aUniformName, const jfc::default_ptr<texture> &atexture){};

        void setMat4x4(const std::string &aUniformName, const graphics_mat4x4_type &agraphics_mat4x4_type){};
            
        void set_model_matrix(const graphics_vector3_type &aWorldPos, 
            const graphics_quaternion_type &aRotation, 
            const graphics_vector3_type &aScale = graphics_vector3_type::One);

        const graphics_mat4x4_type &getmodelMatrix() const;

        /// \brief copy semantics
        model(const model &) = default;
        /// \brief copy semantics
        model &operator=(const model &) = default;

        /// \brief move semantics
        model(model &&) = default;
        /// \brief move semantics
        model &operator=(model &&) = default;

        model(const std::shared_ptr<vertex_data>, const std::shared_ptr<shader_program>);

        ~model() = default;
    };
}

#endif
