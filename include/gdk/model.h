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
    /// \todo I want to limit access to the draw function to be used by camera only... conceptually, a model can only be seen ("drawn") by a camera. but this will introduce a friend, prevent reuse of model outside of this Graphics project, break encaps, make
    /// model harder to read (because now you must also read perspective_camera), just bad things.
    /// 1) Is that a bad thing? yes 2) Is hiding draw(...) from the enduser overkill? depends on use-case 3) Conclusions: another way to accomplish this (hiding from user, showing to camera) would be via interfaces. Perhaps camera draw takes a 
    /// collection of drawables, not models.
    /// then for the end-user, perhaps it should be up to the direct user of this project to decide whether or not to hide draw from them, and to accomplish that via their own model interface that stands between user and an instance of this.
    /// so the accessability of model::draw will be the responsibility to the direc tuser of this project (passing the buck because use cases vary). and tangentally, camera should possibly take a pointer via interface to "drawable"... possibly
    /// model will have siblings in the future (?)
    /// 4) perhaps there should be a new layer of abstraction? model, texture, e.g as they currently exist should be renamed GLmodel, GLtexture e.g since they are so tied to the GL, have a set of interfaces, logical texture, model etc. where the bare minimum
    /// api is exposed. That does make sense.
    class model final
    {
        friend std::ostream &operator<< (std::ostream &, const model &);
            
        std::string m_Name; //!< Resource Identifier
        graphics_mat4x4_type m_modelMatrix; //!< Position in the world
        
        jfc::default_ptr<vertex_data> m_vertex_data;
        jfc::default_ptr<shader_program> m_shader_program;
            
        texture_uniform_collection m_textures;
        float_uniform_collection   m_Floats;
        vector2_uniform_collection m_Vector2Uniforms;
        vector3_uniform_collection m_Vector3Uniforms;
        vector4_uniform_collection m_Vector4Uniforms;
        mat4x4_uniform_collection  m_Mat4x4Uniforms;

    public:
        /// \brief draws the model at its current world position, with respect to a view and projection matrix.
        ///
        /// \detailed generally should not be called by the end user. view, proj, are most easily provided to the model via a camera.
        ///
        /// \param[in] aTimeSinceStart time since the rendering context began. Could be since the app started or the scene loaded, as long as you are consistent it doenst matter.
        /// \param[in] aDeltaTime time since last time draw was called.
        /// \param[in] aViewMatrix the model's world transformation
        /// \param[in] aProjectionMatrix the camera's projection matrix, to be applied to the model to give the appearance of e.g perspective warp
        void draw(const double aTimeSinceStart, const double aDeltaTime, const graphics_mat4x4_type &aViewMatrix, const graphics_mat4x4_type &aProjectionMatrix);

        void setvertex_data(const jfc::default_ptr<vertex_data> &);
        
        void set_texture(const std::string &aUniformName, const jfc::default_ptr<texture> &atexture);
        void setFloat(const std::string &aUniformName, const std::shared_ptr<float> &aFloat);
        void setVector2(const std::string &aUniformName, const std::shared_ptr<graphics_vector2_type> &agraphics_vector2_type);
        void setVector3(const std::string &aUniformName, const std::shared_ptr<graphics_vector3_type> &agraphics_vector3_type);
        void setVector4(const std::string &aUniformName, const std::shared_ptr<graphics_vector4_type> &agraphics_vector4_type);
        void setMat4x4(const std::string &aUniformName, const graphics_mat4x4_type &agraphics_mat4x4_type);
            
        void set_model_matrix(const graphics_vector3_type &aWorldPos, const graphics_quaternion_type &aRotation, const graphics_vector3_type &aScale = graphics_vector3_type::One);
        const graphics_mat4x4_type &getmodelMatrix() const;
            
        model &operator=(const model &) = delete;
        model &operator=(model &&) = delete;
      
        model(const std::string &aName, const jfc::default_ptr<vertex_data> &, const jfc::default_ptr<shader_program> &);
        model();
        model(const model &) = default;
        model(model &&) = default;
        ~model() = default;
    };
        
    std::ostream &operator<<(std::ostream &, const model &);
}

#endif
