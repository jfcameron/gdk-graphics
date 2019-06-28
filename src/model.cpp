// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/camera.h>
#include <gdk/default_ptr.h>
#include <gdk/glh.h>
#include <gdk/mat4x4.h>
#include <gdk/model.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/opengl.h>
#include <gdk/shaderprogram.h>
#include <gdk/vertexdata.h>

#include <nlohmann/json.hpp>

#include <iostream>

using namespace gdk;

static constexpr char TAG[] = "Model";

std::ostream& gdk::operator<<(std::ostream& s, const Model& a)
{
    return s << nlohmann::json
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },
        
        {"m_Name", a.m_Name},
        {"m_ModelMatrix", jfc::insertion_operator_to_nlohmann_json_object(a.m_ModelMatrix)},
        {"m_VertexData", jfc::insertion_operator_to_nlohmann_json_object(a.m_VertexData.lock())},
        {"m_ShaderProgram", jfc::insertion_operator_to_nlohmann_json_object(a.m_ShaderProgram.lock())},
        {"m_Textures", jfc::insertion_operator_to_nlohmann_json_object(a.m_Textures)},
        {"m_Floats", jfc::insertion_operator_to_nlohmann_json_object(a.m_Floats)},
        {"m_Vector2Uniforms", jfc::insertion_operator_to_nlohmann_json_object(a.m_Vector2Uniforms)},
        {"m_Vector3Uniforms", jfc::insertion_operator_to_nlohmann_json_object(a.m_Vector3Uniforms)},
        {"m_Vector4Uniforms", jfc::insertion_operator_to_nlohmann_json_object(a.m_Vector4Uniforms)},
    }
    .dump();
}

Model::Model(const std::string &aName, const default_ptr<VertexData> &aVertexData, const default_ptr<ShaderProgram> &aShaderProgram)
: m_Name(aName)
, m_VertexData(aVertexData)
, m_ShaderProgram(aShaderProgram)
{}

Model::Model()
    : Model("",
            gdk::default_ptr<gdk::VertexData>(static_cast<std::shared_ptr<gdk::VertexData>>(VertexData::Quad)),
            gdk::default_ptr<gdk::ShaderProgram>(static_cast<std::shared_ptr<gdk::ShaderProgram>>(ShaderProgram::AlphaCutOff)))
{}

void Model::draw(const double aTimeSinceStart, const double aDeltaTime, const Mat4x4 &aViewMatrix, const Mat4x4 &aProjectionMatrix)
{
    if (const auto pShader = m_ShaderProgram.lock())
    {
        if (const auto pVertexData = m_VertexData.lock())
        {
            const GLuint programHandle = pShader->useProgram();
    
            //bind this model's uniforms
            m_Textures.bind(programHandle);
            m_Floats  .bind(programHandle);
            m_Vector2Uniforms.bind(programHandle);
            m_Vector3Uniforms.bind(programHandle);
            m_Vector4Uniforms.bind(programHandle);
            m_Mat4x4s .bind(programHandle);
    
            //bind standard uniforms
            const float time      = aTimeSinceStart;//time::sinceStart(); // This is incorrect. needless dependency for a floating value provide this via param or via module interface.
            const float deltaTime = aDeltaTime; //time::getDeltaTime();
        
            const Mat4x4 p = aProjectionMatrix;
            const Mat4x4 v = aViewMatrix;
            const Mat4x4 m = getModelMatrix();
        
            const auto mvp = p * v * m;
        
            glh::Bind1FloatUniform(programHandle, "_DeltaTime",  deltaTime);
            glh::Bind1FloatUniform(programHandle, "_Time",       time     );
            glh::BindMatrix4x4(programHandle,     "_Model",      m        );
            glh::BindMatrix4x4(programHandle,     "_View",       v        );
            glh::BindMatrix4x4(programHandle,     "_Projection", p        );
            glh::BindMatrix4x4(programHandle,     "_MVP",        mvp      );

            pVertexData->draw(programHandle);
                
            //unbind this model's uniforms
            m_Textures.unbind(programHandle);
            m_Floats  .unbind(programHandle);
            m_Vector2Uniforms.unbind(programHandle);
            m_Vector3Uniforms.unbind(programHandle);
            m_Vector4Uniforms.unbind(programHandle);
            m_Mat4x4s .unbind(programHandle);
        }
    }
}

// Accessors
void Model::setTexture(const std::string &aUniformName, const default_ptr<Texture> &aTexture)
{
    m_Textures.put(aUniformName, aTexture);
}

void Model::setFloat(const std::string &aUniformName, const std::shared_ptr<float> &aFloat)
{
    m_Floats.put(aUniformName, aFloat);
}

void Model::setVector2(const std::string &aUniformName, const std::shared_ptr<graphics_vector2_type> &agraphics_vector2_type)
{
    m_Vector2Uniforms.put(aUniformName, agraphics_vector2_type);
}

void Model::setVector3(const std::string &aUniformName, const std::shared_ptr<graphics_vector3_type> &agraphics_vector3_type)
{
    m_Vector3Uniforms.put(aUniformName, agraphics_vector3_type);
}

void Model::setVector4(const std::string &aUniformName, const std::shared_ptr<graphics_vector4_type> &agraphics_vector4_type)
{
    m_Vector4Uniforms.put(aUniformName, agraphics_vector4_type);
}

void Model::setMat4x4(const std::string &aUniformName, const Mat4x4 &aMat4x4 )
{
    m_Mat4x4s.put(aUniformName, aMat4x4);
}

const Mat4x4& Model::getModelMatrix() const
{
    return m_ModelMatrix;
}

void Model::setModelMatrix(const graphics_vector3_type &aWorldPos, const Quaternion &aRotation, const graphics_vector3_type &aScale)
{
    m_ModelMatrix.setToIdentity();
    m_ModelMatrix.translate(aWorldPos);
    m_ModelMatrix.rotate(aRotation);
    m_ModelMatrix.scale(aScale);
}

void Model::setVertexData(const default_ptr<VertexData> &a)
{
    m_VertexData = a;
}