// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <gdk/floatuniformcollection.h>
#include <gdk/mat4x4.h>
#include <gdk/mat4x4uniformcollection.h>
#include <gdk/textureuniformcollection.h>
#include <gdk/vector2uniformcollection.h>
#include <gdk/vector3.h>
#include <gdk/vector3uniformcollection.h>
#include <gdk/vector4uniformcollection.h>
#include <gdk/quaternion.h>

#include <iosfwd>
#include <memory>

namespace gdk
{
    struct VertexData;
    struct ShaderProgram;
    struct Vector2;
    struct Vector4;
    
    /// \brief Represents an observable 3D object. 
    ///
    /// \detailed Contains a VertexData, a set of uniform collections, a shader, animations, a skeleton.
    ///
    /// \warning I think this class contains a bit too much implementation. (see draw method). It seems weird that "Model"
    /// is responsible for binding and clearing all uniform data for the shader.
    ///
    /// \todo the shaderprogram and uniform data (textures etc.)  Should probably
    /// be broken out into a new abstraction. This work would be a good match for the "material" class seen in many engines.
    ///
    /// \todo I want to limit access to the draw function to be used by camera only... conceptually, a model can only be seen ("drawn") by a camera. but this will introduce a friend, prevent reuse of Model outside of this Graphics project, break encaps, make
    /// Model harder to read (because now you must also read Camera), just bad things.
    /// 1) Is that a bad thing? yes 2) Is hiding draw(...) from the enduser overkill? depends on use-case 3) Conclusions: another way to accomplish this (hiding from user, showing to camera) would be via interfaces. Perhaps camera draw takes a 
    /// collection of drawables, not models.
    /// then for the end-user, perhaps it should be up to the direct user of this project to decide whether or not to hide draw from them, and to accomplish that via their own model interface that stands between user and an instance of this.
    /// so the accessability of model::draw will be the responsibility to the direc tuser of this project (passing the buck because use cases vary). and tangentally, camera should possibly take a pointer via interface to "drawable"... possibly
    /// model will have siblings in the future (?)
    /// 4) perhaps there should be a new layer of abstraction? Model, Texture, e.g as they currently exist should be renamed GLModel, GLTexture e.g since they are so tied to the GL, have a set of interfaces, logical Texture, Model etc. where the bare minimum
    /// api is exposed. That does make sense.
    class Model final
    {
        friend std::ostream &operator<< (std::ostream &, const Model &);
            
        std::string m_Name =   {}; //!< Resource Identifier
        Mat4x4 m_ModelMatrix = {}; //!< Position in the world
        
        default_ptr<VertexData> m_VertexData;
        default_ptr<ShaderProgram> m_ShaderProgram;
            
        TextureUniformCollection m_Textures = {};
        FloatUniformCollection   m_Floats   = {};
        Vector2UniformCollection m_Vector2s = {};
        Vector3UniformCollection m_Vector3s = {};
        Vector4UniformCollection m_Vector4s = {};
        Mat4x4UniformCollection  m_Mat4x4s  = {};

    public:
        /// \brief draws the model at its current world position, with respect to a view and projection matrix.
        ///
        /// \detailed generally should not be called by the end user. view, proj, are most easily provided to the model via a camera.
        ///
        /// \param[in] aTimeSinceStart time since the rendering context began. Could be since the app started or the scene loaded, as long as you are consistent it doenst matter.
        /// \param[in] aDeltaTime time since last time draw was called.
        /// \param[in] aViewMatrix the model's world transformation
        /// \param[in] aProjectionMatrix the camera's projection matrix, to be applied to the model to give the appearance of e.g perspective warp
        void draw(const double &aTimeSinceStart, const double &aDeltaTime, const Mat4x4 &aViewMatrix, const Mat4x4 &aProjectionMatrix);

        void setVertexData(const default_ptr<VertexData> &);
        
        void setTexture(const std::string &aUniformName, const default_ptr<Texture>     &aTexture);
        void setFloat  (const std::string &aUniformName, const std::shared_ptr<float>   &aFloat  );
        void setVector2(const std::string &aUniformName, const std::shared_ptr<Vector2> &aVector2);
        void setVector3(const std::string &aUniformName, const std::shared_ptr<Vector3> &aVector3);
        void setVector4(const std::string &aUniformName, const std::shared_ptr<Vector4> &aVector4);
        void setMat4x4 (const std::string &aUniformName, const Mat4x4                   &aMat4x4 );
            
        void setModelMatrix(const Vector3 &aWorldPos, const Quaternion &aRotation, const Vector3 &aScale = Vector3::One);
        const Mat4x4 &getModelMatrix() const;
            
        Model &operator=(const Model &) = delete;
        Model &operator=(Model &&) = delete;
      
        Model(const std::string &aName, const default_ptr<VertexData> &, const default_ptr<ShaderProgram> &);
        Model();
        Model(const Model &) = default;
        Model(Model &&) = default;
        ~Model() = default;
    };
        
    std::ostream &operator<< (std::ostream &, const Model &);
}

#endif
