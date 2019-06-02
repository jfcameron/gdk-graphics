// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GLH_H
#define GDK_GLH_H

//! OpenGL header for gdk
/// includes gl functions from glew and declares C++ friendly helpers in the GLH namespace

#include <gdk/opengl.h>

#include <string>

namespace gdk
{
    struct Color;
    struct IntVector2;
    struct Vector2;
    struct Vector3;
    struct Vector4;
    struct Mat4x4;
}

namespace glh
{
    // gdk-type friendly conveniences
    void ClearColor(const gdk::Color &aColor);
    void Viewport(const gdk::IntVector2 &aPos, const gdk::IntVector2 &aSize);
    void Scissor(const gdk::IntVector2 &aPos, const gdk::IntVector2 &aSize);
    
    // Error detection & logging
    std::string GetShaderInfoLog(const GLuint aShaderStageHandle);
    std::string GetProgramInfoLog(const GLuint aShaderProgramHandle);
    bool GetError(std::string *aErrorCode = nullptr);
/*    //std::vector<std::string> GetErrors();
    //void LogErrors(const bool &aDoNotLogIfNoErrors = false);
    void ClearErrors();*/

    //! Vertex binding
    bool EnableVertexAttribute(const std::string &aAttributeName, const GLuint aProgramHandle, const int aAttributeSize,
                               const int aAttributeOffset, const int aTotalNumberOfVertexAttributeComponents);
    
    bool BindTextureUniform(const GLuint aShaderHandle, const std::string &aUniformName, const GLuint aTextureHandle, const int aTextureUnit);//, final GLenum &aTextureType);
    bool Bind1FloatUniform (const GLuint aShaderHandle, const std::string &aUniformName, const float aScalar);
    bool Bind2FloatUniform (const GLuint aShaderHandle, const std::string &aUniformName, const gdk::Vector2 &aVector2);
    bool Bind3FloatUniform (const GLuint aShaderHandle, const std::string &aUniformName, const gdk::Vector3 &aVector3);
    bool Bind4FloatUniform (const GLuint aShaderHandle, const std::string &aUniformName, const gdk::Vector4 &aVector4);
    bool BindMatrix4x4     (const GLuint aShaderHandle, const std::string &aUniformName, const gdk::Mat4x4  &aMatrix4x4);
}

#endif
