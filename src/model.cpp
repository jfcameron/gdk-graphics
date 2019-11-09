// © 2018 Joseph Cameron - All Rights Reserved

#include <jfc/default_ptr.h>
#include <gdk/glh.h>
#include <gdk/mat4x4.h>
#include <gdk/model.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/opengl.h>
#include <gdk/shader_program.h>
#include <gdk/vertex_data.h>

#include <nlohmann/json.hpp>

#include <iostream>

using namespace gdk;

static constexpr char TAG[] = "model";

std::ostream& gdk::operator<<(std::ostream& s, const model& a)
{
    return s << nlohmann::json
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },
        
        {"m_Name", a.m_Name},
        /*{"m_modelMatrix", jfc::insertion_operator_to_nlohmann_json_object(a.m_modelMatrix)},
        {"m_vertex_data", jfc::insertion_operator_to_nlohmann_json_object(a.m_vertex_data.lock())},
        {"m_shader_program", jfc::insertion_operator_to_nlohmann_json_object(a.m_shader_program.lock())},
        {"m_textures", jfc::insertion_operator_to_nlohmann_json_object(a.m_textures)},
        {"m_Floats", jfc::insertion_operator_to_nlohmann_json_object(a.m_Floats)},
        {"m_Vector2Uniforms", jfc::insertion_operator_to_nlohmann_json_object(a.m_Vector2Uniforms)},
        {"m_Vector3Uniforms", jfc::insertion_operator_to_nlohmann_json_object(a.m_Vector3Uniforms)},
        {"m_Vector4Uniforms", jfc::insertion_operator_to_nlohmann_json_object(a.m_Vector4Uniforms)},*/
    }
    .dump();
}

model::model(const std::string &aName, const jfc::default_ptr<vertex_data> &avertex_data, const jfc::default_ptr<shader_program> &ashader_program)
: m_Name(aName)
, m_vertex_data(avertex_data)
, m_shader_program(ashader_program)
{}

model::model()
    : model("",
            jfc::default_ptr<gdk::vertex_data>(static_cast<std::shared_ptr<gdk::vertex_data>>(vertex_data::Quad)),
            jfc::default_ptr<gdk::shader_program>(static_cast<std::shared_ptr<gdk::shader_program>>(shader_program::AlphaCutOff)))
{}

void model::draw(const double aTimeSinceStart, const double aDeltaTime, const graphics_mat4x4_type &aViewMatrix, const graphics_mat4x4_type &aProjectionMatrix)
{
    if (const auto pShader = m_shader_program.lock()) //This function is not "try_draw" it is "draw". logical contract between user and implementor cannot be complete in the null case. except if null.
    {
        if (const auto pvertex_data = m_vertex_data.lock())
        {
            const GLuint programHandle = pShader->useProgram();
    
            //bind this model's uniforms
            m_textures.bind(programHandle);
            m_Floats.bind(programHandle);
            m_Vector2Uniforms.bind(programHandle);
            m_Vector3Uniforms.bind(programHandle);
            m_Vector4Uniforms.bind(programHandle);
            m_Mat4x4Uniforms.bind(programHandle);
    
            //bind standard uniforms
            const float time = aTimeSinceStart;//time::sinceStart(); // This is incorrect. needless dependency for a floating value provide this via param or via module interface.
            const float deltaTime = aDeltaTime; //time::getDeltaTime();
        
            const graphics_mat4x4_type p = aProjectionMatrix;
            const graphics_mat4x4_type v = aViewMatrix;
            const graphics_mat4x4_type m = getmodelMatrix();
        
            const auto mvp = p * v * m;
        
            glh::Bind1FloatUniform(programHandle, "_DeltaTime",  deltaTime);
            glh::Bind1FloatUniform(programHandle, "_Time",       time     );
            glh::BindMatrix4x4(programHandle,     "_model",      m        );
            glh::BindMatrix4x4(programHandle,     "_View",       v        );
            glh::BindMatrix4x4(programHandle,     "_Projection", p        );
            glh::BindMatrix4x4(programHandle,     "_MVP",        mvp      );

            pvertex_data->draw(programHandle);
                
            //unbind this model's uniforms
            m_textures.unbind(programHandle);
            m_Floats.unbind(programHandle);
            m_Vector2Uniforms.unbind(programHandle);
            m_Vector3Uniforms.unbind(programHandle);
            m_Vector4Uniforms.unbind(programHandle);
            m_Mat4x4Uniforms.unbind(programHandle);
        }
    }
}

// Accessors
void model::set_texture(const std::string &aUniformName, const jfc::default_ptr<texture> &atexture)
{
    m_textures.insert(aUniformName, atexture);
}

void model::setFloat(const std::string &aUniformName, const std::shared_ptr<float> &aFloat)
{
    m_Floats.insert(aUniformName, aFloat);
}

void model::setVector2(const std::string &aUniformName, const std::shared_ptr<graphics_vector2_type> &agraphics_vector2_type)
{
    m_Vector2Uniforms.insert(aUniformName, agraphics_vector2_type);
}

void model::setVector3(const std::string &aUniformName, const std::shared_ptr<graphics_vector3_type> &agraphics_vector3_type)
{
    m_Vector3Uniforms.insert(aUniformName, agraphics_vector3_type);
}

void model::setVector4(const std::string &aUniformName, const std::shared_ptr<graphics_vector4_type> &agraphics_vector4_type)
{
    m_Vector4Uniforms.insert(aUniformName, agraphics_vector4_type);
}

void model::setMat4x4(const std::string &aUniformName, const graphics_mat4x4_type &agraphics_mat4x4_type )
{
    m_Mat4x4Uniforms.insert(aUniformName, agraphics_mat4x4_type);
}

const graphics_mat4x4_type& model::getmodelMatrix() const
{
    return m_modelMatrix;
}

void model::set_model_matrix(const graphics_vector3_type &aWorldPos, const graphics_quaternion_type &aRotation, const graphics_vector3_type &aScale)
{
    m_modelMatrix.setToIdentity();
    m_modelMatrix.translate(aWorldPos);
    m_modelMatrix.rotate(aRotation);
    m_modelMatrix.scale(aScale);
}

void model::setvertex_data(const jfc::default_ptr<vertex_data> &a)
{
    m_vertex_data = a;
}
