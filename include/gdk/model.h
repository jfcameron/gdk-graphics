// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <gdk/float_uniform_collection.h>
#include <gdk/graphics_types.h>
#include <gdk/mat4x4_uniform_collection.h>
#include <gdk/texture_uniform_collection.h>
#include <gdk/vector2_uniform_collection.h>
#include <gdk/vector3_uniform_collection.h>
#include <gdk/vector4_uniform_collection.h>

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
    /// \TODO break up all the uniform binding! Simplify!
    class model final
    {
        //! Position in the world
        graphics_mat4x4_type m_modelMatrix; 
        
        std::shared_ptr<vertex_data> m_vertex_data;
        std::shared_ptr<shader_program> m_ShaderProgram;
            
        texture_uniform_collection m_textures;
        float_uniform_collection   m_Floats;
        vector2_uniform_collection m_Vector2Uniforms;
        vector3_uniform_collection m_Vector3Uniforms;
        vector4_uniform_collection m_Vector4Uniforms;
        mat4x4_uniform_collection  m_Mat4x4Uniforms;

    public:
        /// \brief draws the model at its current world position, with respect to a view and projection matrix.
        /// generally should not be called by the end user. view, proj, are most easily provided to the model via a camera.
        void draw(const double aTimeSinceStart, 
            const double aDeltaTime, 
            const graphics_mat4x4_type &aViewMatrix, 
            const graphics_mat4x4_type &aProjectionMatrix) const;

        void setvertex_data(const std::shared_ptr<vertex_data>);
        
        void set_texture(const std::string &aUniformName, const jfc::default_ptr<texture> &atexture);
        void setFloat(const std::string &aUniformName, const std::shared_ptr<float> &aFloat);
        void setVector2(const std::string &aUniformName, const std::shared_ptr<graphics_vector2_type> &agraphics_vector2_type);
        void setVector3(const std::string &aUniformName, const std::shared_ptr<graphics_vector3_type> &agraphics_vector3_type);
        void setVector4(const std::string &aUniformName, const std::shared_ptr<graphics_vector4_type> &agraphics_vector4_type);
        void setMat4x4(const std::string &aUniformName, const graphics_mat4x4_type &agraphics_mat4x4_type);
            
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
