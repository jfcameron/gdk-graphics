// © 2018 Joseph Cameron - All Rights Reserved

#include <jfc/default_ptr.h>
#include <gdk/glh.h>
#include <gdk/mat4x4.h>
#include <gdk/opengl.h>

#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_shader_program.h>
#include <gdk/webgl1es2_model.h>

using namespace gdk;

static constexpr char TAG[] = "entity";

webgl1es2_entity::webgl1es2_entity(const std::shared_ptr<webgl1es2_model> amodel, const std::shared_ptr<webgl1es2_material> aMaterial)
: m_model(amodel)
, m_Material(aMaterial)
{}

void webgl1es2_entity::draw(const graphics_mat4x4_type &aViewMatrix, const graphics_mat4x4_type &aProjectionMatrix) const
{
    // TODO: refactor upwards, then propegate the handle down so MVP can be bound       
    // program should be installed to the pipeline in the "pipeline" abstraction
    //const GLuint programHandle = m_ShaderProgram->useProgram();
    
    //TODO Refactor to .. scene? batch?
    // these uniforms belong to a higherlevel abstraction. Higher than webgl1es2_entityor camera. should be "scene".
    //bind standard uniforms
    /*const float time = aTimeSinceStart;
    const float deltaTime = aDeltaTime;

    glh::Bind1FloatUniform(programHandle, "_DeltaTime",  deltaTime);
    glh::Bind1FloatUniform(programHandle, "_Time",       time     );*/

    // Refactor to material
    //bind this webgl1es2_entity's uniforms
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

    m_Material->m_pShaderProgram->setUniform("_Model", mvp); // HMMMMM. Incorrect. should be m_pMaterial->m_ShaderProgram->setUniform(...) or even better m_pMaterial->setUniform(...). or hm. ahh not sure
    m_Material->m_pShaderProgram->setUniform("_View", mvp);
    m_Material->m_pShaderProgram->setUniform("_Projection", mvp);
    m_Material->m_pShaderProgram->setUniform("_MVP", mvp);

    // TODO Refactor this to "batch" abstraction
    //m_model->bind(*m_ShaderProgram);

    // Acceptable!
    m_model->draw();
}

const graphics_mat4x4_type &webgl1es2_entity::getModelMatrix() const
{
    return m_ModelMatrix;
}

void webgl1es2_entity::set_model_matrix(const graphics_vector3_type &aWorldPos, const graphics_quaternion_type &aRotation, const graphics_vector3_type &aScale)
{
    m_ModelMatrix.setToIdentity();
    m_ModelMatrix.translate(aWorldPos);
    m_ModelMatrix.rotate(aRotation);
    m_ModelMatrix.scale(aScale);
}

void webgl1es2_entity::set_model(const std::shared_ptr<webgl1es2_model> a)
{
    m_model = a;
}

