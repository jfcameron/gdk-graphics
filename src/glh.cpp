// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/color.h>
#include <gdk/glh.h>

#include <nlohmann/json.hpp>

#include <vector>

namespace glh
{
    void Clearcolor(const gdk::color &acolor)
    {
        glClearColor(acolor.r, acolor.g, acolor.b, acolor.a);
    }

    bool Enablevertex_attribute(const std::string_view &aAttributeName, const GLuint aProgramHandle, const int aAttributeSize, const int aAttributeOffset, const int aTotalNumberOfvertex_attributeComponents)
    {
        GLint attribute = glGetAttribLocation(aProgramHandle, aAttributeName.data());
    
        if (attribute == -1) return false; // The attribute either does not exist or is not used in the current shader program
         
        glEnableVertexAttribArray(attribute);
    
        //Create vertex attribute pointers..
        glVertexAttribPointer(
            attribute,      //Position attribute index
            aAttributeSize, //Pos size
            GL_FLOAT,       //data type of each member of the format (must be uniform, look at glbindbufferdata, it takes an array or ptr to an array, so no suprise)
            GL_FALSE,
            sizeof(GLfloat)*aTotalNumberOfvertex_attributeComponents,
            reinterpret_cast<void *>(sizeof(GLfloat) * aAttributeOffset));
    
        return true;
    }

    void Viewport(const gdk::graphics_intvector2_type& aPos, const gdk::graphics_intvector2_type& aSize)
    {
        glViewport(aPos.x, aPos.y, aSize.x, aSize.y);
    }

    void Scissor(const gdk::graphics_intvector2_type& aPos, const gdk::graphics_intvector2_type& aSize)
    {
        glScissor(aPos.x, aPos.y, aSize.x, aSize.y);
    }

    bool Bind1FloatUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const float aValue)
    {
        GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.data());
        
        if (uniformHandle == -1) return false;
        
        glUniform1f(uniformHandle, aValue);
        
        return true;
    }

    bool Bind2FloatUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const gdk::graphics_vector2_type &agraphics_vector2_type)
    {
        GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.data());
        
        if (uniformHandle == -1) return false;
        
        glUniform2f(uniformHandle, agraphics_vector2_type.x, agraphics_vector2_type.y);
        
        return true;
    }

    bool Bind3FloatUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const gdk::graphics_vector3_type &agraphics_vector3_type)
    {
        GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.data());
        
        if (uniformHandle == -1) return false;
        
        glUniform3f(uniformHandle, agraphics_vector3_type.x, agraphics_vector3_type.y, agraphics_vector3_type.z);
        
        return true;
    }

    bool Bind4FloatUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const gdk::graphics_vector4_type &agraphics_vector4_type)
    {
        GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.data());
        
        if (uniformHandle == -1) return false;
        
        glUniform4f(uniformHandle, agraphics_vector4_type.x, agraphics_vector4_type.y, agraphics_vector4_type.z, agraphics_vector4_type.w);
        
        return true;
    }

    bool BindMatrix4x4(const GLuint aShaderHandle, const std::string_view &aUniformName, const gdk::graphics_mat4x4_type &aMatrix4x4)
    {
        GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.data());
        
        if (uniformHandle == -1) return false;
        
        glUniformMatrix4fv(uniformHandle, 1, GL_FALSE, &aMatrix4x4.m[0][0]);
        
        return true;
    }

    bool BindtextureUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const GLuint atextureHandle, const int atextureUnit)
    {
        GLint uniformHandle  = glGetUniformLocation(aShaderHandle, aUniformName.data());
        int thetextureType = GL_TEXTURE_2D;
        
        if (uniformHandle == -1) return false;
        
        switch (atextureUnit)
        {
            case 1: glActiveTexture(GL_TEXTURE1); break;
            case 2: glActiveTexture(GL_TEXTURE2); break;
            case 3: glActiveTexture(GL_TEXTURE3); break;
            case 4: glActiveTexture(GL_TEXTURE4); break;
            case 5: glActiveTexture(GL_TEXTURE5); break;
            case 6: glActiveTexture(GL_TEXTURE6); break;
            case 7: glActiveTexture(GL_TEXTURE7); break;
                
            default: glActiveTexture( GL_TEXTURE0); break;
        }
        
        glBindTexture(thetextureType, atextureHandle);
        glUniform1i(uniformHandle, atextureUnit);
        
        return true;
    }

    std::string GetShaderInfoLog(const GLuint aShaderStageHandle)
    {
        GLint bufflen = 0;
        glGetShaderiv(aShaderStageHandle, GL_INFO_LOG_LENGTH, &bufflen);
        
        if (bufflen > 1)
        {
            std::vector<GLchar> infoLog(bufflen);
            glGetShaderInfoLog(aShaderStageHandle, bufflen, 0, &infoLog[0]);
            
            return std::string(infoLog.begin(),infoLog.end());
        }
        
        return "clear";
    }

    std::string GetProgramInfoLog(const GLuint ashader_programHandle)
    {
        GLint maxLength = 0;
        glGetProgramiv(ashader_programHandle, GL_INFO_LOG_LENGTH, &maxLength);
        
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(ashader_programHandle, maxLength, &maxLength, &infoLog[0]);
        
        return std::string(infoLog.begin(),infoLog.end());
    }
}
///////////////////////////////////////////////////////////////////////
/*
// © 2018 Joseph Cameron - All Rights Reserved
// Project: gdk
// Created on 17-07-02.
#include <GL.h>
//gdk inc
#include <color.h>
#include <Debug/Logger.h>]
#include <Math/graphics_intvector2_type.h>
#include <Math/graphics_mat4x4_type.h>
#include <Math/graphics_vector2_type.h>
#include <Math/graphics_vector3_type.h>
#include <Math/graphics_vector4_type.h>
//std inc
#include <vector>
using namespace gdk;


void glh::Viewport(const gdk::graphics_intvector2_type& aPos, const gdk::graphics_intvector2_type& aSize)
{
glViewport(aPos.x, aPos.y, aSize.x, aSize.y);
}

void glh::Scissor(const gdk::graphics_intvector2_type& aPos, const gdk::graphics_intvector2_type& aSize)
{
glScissor(aPos.x, aPos.y, aSize.x, aSize.y);
}

void glh::Clearcolor(const gdk::color &acolor)
{
    glClearColor(acolor.r, acolor.g, acolor.b, acolor.a);
}
*/
bool glh::GetError(std::string *aErrorCode)
{
    std::string errorcodebuffer = "";
    
    switch (glGetError())
    {
        case(GL_NO_ERROR):
            errorcodebuffer = "GL_NO_ERROR";
            return false;
        
        case(GL_INVALID_ENUM):
            errorcodebuffer = "GL_INVALID_ENUM";
            
        case(GL_INVALID_VALUE):
            errorcodebuffer = "GL_INVALID_VALUE";
            
        case(GL_INVALID_OPERATION):
            errorcodebuffer = "GL_INVALID_OPERATION";
            
        case(GL_INVALID_FRAMEBUFFER_OPERATION):
            errorcodebuffer = "GL_INVALID_FRAMEBUFFER_OPERATION";
        
        case(GL_OUT_OF_MEMORY):
            errorcodebuffer = "GL_OUT_OF_MEMORY";
        
        default:
            errorcodebuffer = "gdk_UNHANDLED_GL_ERROR_CODE";
        
    }
    
    if (aErrorCode != nullptr)
    {
        *aErrorCode = errorcodebuffer;
    }
    
    return true;
}

/*std::vector<std::string_view> glh::GetErrors()
{
    std::vector<std::string_view> errors;
    
    for(std::string_view error=GetError();;error=GetError())
    {
        errors.push_back(error);
        
        if (errors.back()=="GL_NO_ERROR")
            break;
        
    }
    
    return errors;
    
}*/

/*void glh::LogErrors(const bool &aDoNotLogIfNoErrors)
{
    std::vector<std::string_view> errors = GetErrors();
    size_t s = errors.size();
    
    if (aDoNotLogIfNoErrors == true && s <= 1)
        return;
    
    std::ostringstream ss;
    ss << "OpenGL errors: ";
    
    if (s > 1) //removes GL_NO_ERROR from being written at the end of the log
        s--;
    
    for(size_t i=0;i<s;i++)
    {
        ss << errors[i];
        
        if (i != s-1)
            ss << ", ";
        
    }
    
    Debug::error(TAG, ss.str());
    
}*/
/*
void glh::ClearErrors()
{
    while (glGetError() != GL_NO_ERROR);
}

bool glh::BindtextureUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const GLuint atextureHandle, const int atextureUnit*//*, final GLenum &atextureType*//*)
{
    GLint uniformHandle  = glGetUniformLocation(aShaderHandle, aUniformName.c_str());
    int thetextureType = GL_TEXTURE_2D;
    
    if (uniformHandle == -1)
        return false;
    
    switch (atextureUnit)
    {
        case 1: glActiveTexture(GL_TEXTURE1); break;
        case 2: glActiveTexture(GL_TEXTURE2); break;
        case 3: glActiveTexture(GL_TEXTURE3); break;
        case 4: glActiveTexture(GL_TEXTURE4); break;
        case 5: glActiveTexture(GL_TEXTURE5); break;
        case 6: glActiveTexture(GL_TEXTURE6); break;
        case 7: glActiveTexture(GL_TEXTURE7); break;
            
        default: glActiveTexture( GL_TEXTURE0); break;
    }
    
    glBindTexture(thetextureType, atextureHandle);
    glUniform1i(uniformHandle, atextureUnit);
    
    return true;
}

bool glh::Bind1FloatUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const float aValue)
{
    GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.c_str());
    
    if (uniformHandle == -1)
        return false;
    
    glUniform1f(uniformHandle, aValue);
    
    return true;
}

bool glh::Bind2FloatUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const gdk::graphics_vector2_type &agraphics_vector2_type)
{
    GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.c_str());
    
    if (uniformHandle == -1)
        return false;
    
    glUniform2f(uniformHandle, agraphics_vector2_type.x, agraphics_vector2_type.y);
    
    return true;
}

bool glh::Bind3FloatUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const gdk::graphics_vector3_type &agraphics_vector3_type)
{
    GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.c_str());
    
    if (uniformHandle == -1)
        return false;
    
    glUniform3f(uniformHandle, agraphics_vector3_type.x, agraphics_vector3_type.y, agraphics_vector3_type.z);
    
    return true;
}

bool glh::Bind4FloatUniform(const GLuint aShaderHandle, const std::string_view &aUniformName, const gdk::graphics_vector4_type &agraphics_vector4_type)
{
    GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.c_str());
    
    if (uniformHandle == -1)
        return false;
    
    glUniform4f(uniformHandle, agraphics_vector4_type.x, agraphics_vector4_type.y, agraphics_vector4_type.z, agraphics_vector4_type.w);
    
    return true;
}

bool glh::BindMatrix4x4(const GLuint aShaderHandle, const std::string_view &aUniformName, const gdk::graphics_mat4x4_type &aMatrix4x4)
{
    GLint uniformHandle = glGetUniformLocation(aShaderHandle, aUniformName.c_str());
    
    if (uniformHandle == -1)
        return false;
    
    glUniformMatrix4fv(uniformHandle, 1, GL_FALSE, &aMatrix4x4.m[0][0]);
    
    return true;
}
*/
