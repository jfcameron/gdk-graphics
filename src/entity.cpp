// © 2018 Joseph Cameron - All Rights Reserved

#include <jfc/default_ptr.h>
#include <gdk/glh.h>
#include <gdk/mat4x4.h>
#include <gdk/entity.h>
#include <gdk/opengl.h>
#include <gdk/shader_program.h>
#include <gdk/model.h>

using namespace gdk;

static constexpr char TAG[] = "entity";

entity::entity(const std::shared_ptr<model> amodel, const std::shared_ptr<shader_program> ashader_program)
: m_model(amodel)
, m_ShaderProgram(ashader_program)
{}

void entity::draw(const graphics_mat4x4_type &aViewMatrix, const graphics_mat4x4_type &aProjectionMatrix) const
{
    // TODO: refactor upwards, then propegate the handle down so MVP can be bound       
    // program should be installed to the pipeline in the "pipeline" abstraction
    const GLuint programHandle = m_ShaderProgram->useProgram();
    
    //TODO Refactor to .. scene? batch?
    // these uniforms belong to a higherlevel abstraction. Higher than entityor camera. should be "scene".
    //bind standard uniforms
    /*const float time = aTimeSinceStart;
    const float deltaTime = aDeltaTime;

    glh::Bind1FloatUniform(programHandle, "_DeltaTime",  deltaTime);
    glh::Bind1FloatUniform(programHandle, "_Time",       time     );*/

    // Refactor to material
    //bind this entity's uniforms
    /*m_textures.bind(programHandle);
    m_Floats.bind(programHandle);
    m_Vector2Uniforms.bind(programHandle);
    m_Vector3Uniforms.bind(programHandle);
    m_Vector4Uniforms.bind(programHandle);
    m_Mat4x4Uniforms.bind(programHandle);*/
    
    // Acceptable!
    const graphics_mat4x4_type p = aProjectionMatrix;
    const graphics_mat4x4_type v = aViewMatrix;
    const graphics_mat4x4_type m = getModelMatrix();

    const auto mvp = p * v * m;

    m_ShaderProgram->setUniform("_Model", mvp);
    m_ShaderProgram->setUniform("_View", mvp);
    m_ShaderProgram->setUniform("_Projection", mvp);
    m_ShaderProgram->setUniform("_MVP", mvp);

    // TODO Refactor this to "batch" abstraction
    m_model->bind(*m_ShaderProgram);

    // Acceptable!
    m_model->draw();
}

const graphics_mat4x4_type &entity::getModelMatrix() const
{
    return m_ModelMatrix;
}

void entity::set_model_matrix(const graphics_vector3_type &aWorldPos, const graphics_quaternion_type &aRotation, const graphics_vector3_type &aScale)
{
    m_ModelMatrix.setToIdentity();
    m_ModelMatrix.translate(aWorldPos);
    m_ModelMatrix.rotate(aRotation);
    m_ModelMatrix.scale(aScale);
}

void entity::set_model(const std::shared_ptr<model> a)
{
    m_model = a;
}

