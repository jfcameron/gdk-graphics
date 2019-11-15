// © 2018 Joseph Cameron - All Rights Reserved

#include <jfc/default_ptr.h>
#include <gdk/glh.h>
#include <gdk/mat4x4.h>
#include <gdk/model.h>
#include <gdk/opengl.h>
#include <gdk/shader_program.h>
#include <gdk/vertex_data.h>

using namespace gdk;

static constexpr char TAG[] = "model";

model::model(const std::shared_ptr<vertex_data> avertex_data, const std::shared_ptr<shader_program> ashader_program)
: m_vertex_data(avertex_data)
, m_ShaderProgram(ashader_program)
{}

void model::draw(
    //const shader_program &aCurrentShaderProgram, // Required to assign uniforms 

    const double aTimeSinceStart, 
    const double aDeltaTime, 

    const graphics_mat4x4_type &aViewMatrix, 
    const graphics_mat4x4_type &aProjectionMatrix) const
{
    auto pvertex_data = m_vertex_data;
       
    // refactor upwards, then propegate the handle down so MVP can be bound       
    const GLuint programHandle = m_ShaderProgram->useProgram();
    
    // Acceptable!
    const graphics_mat4x4_type p = aProjectionMatrix;
    const graphics_mat4x4_type v = aViewMatrix;
    const graphics_mat4x4_type m = getmodelMatrix();

    const auto mvp = p * v * m;

    m_ShaderProgram->setUniform("_Model", mvp);
    m_ShaderProgram->setUniform("_View", mvp);
    m_ShaderProgram->setUniform("_Projection", mvp);
    m_ShaderProgram->setUniform("_MVP", mvp);

    //-=-=-==-=--=-==--= Refactor to .. scene? batch?
    // these uniforms belong to a higherlevel abstraction. Higher than modelor camera. should be "scene".
    //bind standard uniforms
    const float time = aTimeSinceStart;
    const float deltaTime = aDeltaTime;

    /*glh::Bind1FloatUniform(programHandle, "_DeltaTime",  deltaTime);
    glh::Bind1FloatUniform(programHandle, "_Time",       time     );*/
    //-=-=-==-=--=-==--=

    // =--==--==-= Refactor to material
    //bind this model's uniforms
    /*m_textures.bind(programHandle);
    m_Floats.bind(programHandle);
    m_Vector2Uniforms.bind(programHandle);
    m_Vector3Uniforms.bind(programHandle);
    m_Vector4Uniforms.bind(programHandle);
    m_Mat4x4Uniforms.bind(programHandle);*/
    //-=-=-==-=--=-==--=

    // -=-=-=-=-== Split in two (1) setting up vertex stuff, 2) draw) then do set up once per batch
    pvertex_data->bind(*m_ShaderProgram); // inversion here. VertexData, format pointers etc shoul be done before this (once per batch)

    pvertex_data->draw(); // inversion here. VertexData, format pointers etc shoul be done before this (once per batch)
}

// Accessors
/*void model::set_texture(const std::string &aUniformName, const jfc::default_ptr<texture> &atexture)
{
    //m_textures.insert(aUniformName, atexture);
}

void model::setFloat(const std::string &aUniformName, const std::shared_ptr<float> &aFloat)
{
    //m_Floats.insert(aUniformName, aFloat);
}

void model::setVector2(const std::string &aUniformName, const std::shared_ptr<graphics_vector2_type> &agraphics_vector2_type)
{
    //m_Vector2Uniforms.insert(aUniformName, agraphics_vector2_type);
}

void model::setVector3(const std::string &aUniformName, const std::shared_ptr<graphics_vector3_type> &agraphics_vector3_type)
{
    //m_Vector3Uniforms.insert(aUniformName, agraphics_vector3_type);
}

void model::setVector4(const std::string &aUniformName, const std::shared_ptr<graphics_vector4_type> &agraphics_vector4_type)
{
    //m_Vector4Uniforms.insert(aUniformName, agraphics_vector4_type);
}

void model::setMat4x4(const std::string &aUniformName, const graphics_mat4x4_type &agraphics_mat4x4_type )
{
    //m_Mat4x4Uniforms.insert(aUniformName, agraphics_mat4x4_type);
}*/

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

void model::setvertex_data(const std::shared_ptr<vertex_data> a)
{
    m_vertex_data = a;
}

