// © Joseph Cameron - All Rights Reserved

#include <gdk/color.h>
#include <gdk/glh.h>
#include <gdk/graphics_exception.h>

#include <cmath>
#include <vector>

namespace glh {
    void ClearColor(const gdk::color &acolor) {
        glClearColor(acolor.r, acolor.g, acolor.b, acolor.a);
    }

    void EnableVertexAttribute(const GLint attributeLocation, 
        const int aAttributeSize, 
        const int aAttributeOffset, 
        const int aTotalNumberOfvertex_attributeComponents) {
        glEnableVertexAttribArray(attributeLocation);
    
        glVertexAttribPointer(
            attributeLocation, //Position attribute index
            aAttributeSize,    //Pos size
            GL_FLOAT,          //data type of each member of the format (must be uniform, look at glbindbufferdata, 
            GL_FALSE,          //it takes an array or ptr to an array, so no suprise) TODO: support different types.
            sizeof(GLfloat) * aTotalNumberOfvertex_attributeComponents,
            reinterpret_cast<void *>(sizeof(GLfloat) * aAttributeOffset));
    }

    void Viewport(const gdk::graphics_intvector2_type& aPos, const gdk::graphics_intvector2_type& aSize) {
        glViewport(aPos.x, aPos.y, aSize.x, aSize.y);
    }

    void Scissor(const gdk::graphics_intvector2_type& aPos, const gdk::graphics_intvector2_type& aSize) {
        glScissor(aPos.x, aPos.y, aSize.x, aSize.y);
    }

    void Bind1FloatUniform(const GLint uniformHandle, const float aValue) {
        glUniform1f(uniformHandle, aValue);
    }

    void Bind2FloatUniform(const GLint uniformHandle, const gdk::graphics_vector2_type &agraphics_vector2_type) {
        glUniform2f(uniformHandle, agraphics_vector2_type.x, agraphics_vector2_type.y);
    }

    void Bind3FloatUniform(const GLint uniformHandle, const gdk::graphics_vector3_type &agraphics_vector3_type) {
        glUniform3f(uniformHandle, agraphics_vector3_type.x, agraphics_vector3_type.y, agraphics_vector3_type.z);
    }

    void Bind4FloatUniform(const GLint uniformHandle, const gdk::graphics_vector4_type &agraphics_vector4_type) {
        glUniform4f(uniformHandle, agraphics_vector4_type.x, agraphics_vector4_type.y, agraphics_vector4_type.z, 
			agraphics_vector4_type.w);
    }

    void BindMatrix4x4(const GLint uniformHandle, const gdk::graphics_matrix4x4_type &aMatrix4x4) {
        glUniformMatrix4fv(uniformHandle, 1, GL_FALSE, &aMatrix4x4.front());
    }

    void BindTextureUniform(const GLuint aUniformHandle, const GLuint aTextureHandle, const int aTextureUnit) {
        decltype(GL_TEXTURE_2D) thetextureType(GL_TEXTURE_2D); //TODO: parameterize! Improve texture as well to support non2ds
        
        switch (aTextureUnit) {
            case 0: glActiveTexture(GL_TEXTURE0); break;
            case 1: glActiveTexture(GL_TEXTURE1); break;
            case 2: glActiveTexture(GL_TEXTURE2); break;
            case 3: glActiveTexture(GL_TEXTURE3); break;
            case 4: glActiveTexture(GL_TEXTURE4); break;
            case 5: glActiveTexture(GL_TEXTURE5); break;
            case 6: glActiveTexture(GL_TEXTURE6); break;
            case 7: glActiveTexture(GL_TEXTURE7); break;

            default: throw gdk::graphics_exception(
				"GLES2.0/WebGL1.0 only guarantee 8 texture units; "
				"you are trying to bind too many textures simultaneously to the context");
        }
        
        glBindTexture(thetextureType, aTextureHandle);

        glUniform1i(aUniformHandle, aTextureUnit);
    }

    std::string GetShaderInfoLog(const GLuint aShaderStageHandle) {
        GLint bufflen(0);
        glGetShaderiv(aShaderStageHandle, GL_INFO_LOG_LENGTH, &bufflen);
        
        if (bufflen > 1) {
            std::vector<GLchar> infoLog(bufflen);
            glGetShaderInfoLog(aShaderStageHandle, bufflen, 0, &infoLog[0]);
            
            return std::string(infoLog.begin(),infoLog.end());
        }
        
		return {};
    }

    std::string GetProgramInfoLog(const GLuint ashader_programHandle) {
        GLint maxLength(0);
        glGetProgramiv(ashader_programHandle, GL_INFO_LOG_LENGTH, &maxLength);
        
		if (maxLength) {
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(ashader_programHandle, maxLength, &maxLength, &infoLog[0]);

			return std::string(infoLog.begin(), infoLog.end());
		}

		return {};
    }
}

std::optional<std::string> glh::GetError() {
    switch (glGetError()) {
        case(GL_NO_ERROR): return {};
        case(GL_INVALID_ENUM): return {"GL_INVALID_ENUM"};
        case(GL_INVALID_VALUE): return {"GL_INVALID_VALUE"};
        case(GL_INVALID_OPERATION): return {"GL_INVALID_OPERATION"};
        case(GL_INVALID_FRAMEBUFFER_OPERATION): return {"GL_INVALID_FRAMEBUFFER_OPERATION"};
        case(GL_OUT_OF_MEMORY): return {"GL_OUT_OF_MEMORY"};

        default: break;
    }
    throw gdk::graphics_exception("unhandled gl error type");
}

