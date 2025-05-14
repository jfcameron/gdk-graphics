// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GLH_H
#define GDK_GLH_H

#include <gdk/graphics_types.h>
#include <gdk/opengl.h>
#include <jfc/unique_handle.h>

#include <optional>
#include <string>
#include <string_view>

namespace gdk {
    struct color;
}

namespace glh {
    void ClearColor(const gdk::color &acolor);

    void Viewport(const gdk::graphics_intvector2_type &aPos, 
        const gdk::graphics_intvector2_type &aSize);
    
    void Scissor(const gdk::graphics_intvector2_type &aPos, 
        const gdk::graphics_intvector2_type &aSize);
    
    // Error detection & logging
    std::string GetShaderInfoLog(const GLuint aShaderStageHandle);
    std::string GetProgramInfoLog(const GLuint ashader_programHandle);
    std::optional<std::string> GetError();

    //! Prepares the vertex data for drawing: enables the attribute array, 
    /// sets up the attribute pointers for the used shader program
    void EnableVertexAttribute(const GLint attributeLocation, 
        const int aAttributeSize, 
        const int aAttributeOffset, 
        const int aTotalNumberOfvertex_attributeComponents);

    //! provides texture data to the used shader program
    /// @aUniformHandle handle to the active texture uniform in the used shader program
    void BindTextureUniform(const GLuint aUniformHandle, const GLuint atextureHandle, 
        const int atextureUnit);//, final GLenum &atextureType);

    //! assigns float value to a float uniform at uniform handle within the currently 
    /// used program
    void Bind1FloatUniform(const GLint uniformHandle, const float aScalar);

    //! assigns float2 value to a float2 uniform at uniform handle within the currently 
    /// used program
    void Bind2FloatUniform(const GLint uniformHandle, const gdk::graphics_vector2_type &agraphics_vector2_type);

    //! assigns float3 value to a float3 uniform at uniform handle within the currently 
    /// used program
    void Bind3FloatUniform(const GLint uniformHandle, const gdk::graphics_vector3_type &agraphics_vector3_type);

    //! assigns float4 value to a float4 uniform at uniform handle within the currently 
    /// used program
    void Bind4FloatUniform(const GLint uniformHandle, const gdk::graphics_vector4_type &agraphics_vector4_type);

    //! assigns mat4x4 value to a mat4x4 uniform at uniform handle within the currently 
    /// used program
    void BindMatrix4x4(const GLint uniformHandle, const gdk::graphics_matrix4x4_type &aMatrix4x4);
}

#endif

