// © Joseph Cameron - All Rights Reserved

#include <gdk/webgl1es2_material.h>

#include <iostream>

using namespace gdk;


static inline void setUpFaceCullingMode(material::FaceCullingMode a)
{
    if (a == material::FaceCullingMode::None)
    {
        glDisable(GL_CULL_FACE);

        return;
    }

    glEnable(GL_CULL_FACE);

    switch(a)
    {
        case material::FaceCullingMode::Front: glCullFace(GL_FRONT); break;
        case material::FaceCullingMode::Back: glCullFace(GL_BACK); break;
        case material::FaceCullingMode::FrontAndBack: glCullFace(GL_FRONT_AND_BACK); break;

        case material::FaceCullingMode::None: break;
    }
}

static inline void setRenderMode(material::render_mode aRenderMode)
{
    switch(aRenderMode)
    {
        case material::render_mode::opaque: 
            glDisable(GL_BLEND); break;

        case material::render_mode::transparent:
        {
            glEnable(GL_BLEND);
            
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } break;

        default: std::cout << "not handled!\n"; break;
    }
}

webgl1es2_material::webgl1es2_material(shader_ptr_type pShader,
    material::FaceCullingMode aFaceCullingMode,
    material::render_mode aRenderMode)
: m_pShaderProgram(pShader)
, m_FaceCullMode(aFaceCullingMode)
, m_RenderMode(aRenderMode)
{}


material::render_mode webgl1es2_material::get_render_mode() const
{
    return m_RenderMode;
}

void webgl1es2_material::activate()
{
        setUpFaceCullingMode(m_FaceCullMode);
        setRenderMode(m_RenderMode);

	m_pShaderProgram->useProgram();

	for (const auto& [name, a] : m_Textures) m_pShaderProgram->try_set_uniform(name, *a);
	
        for (const auto& [name, a] : m_Floats)   m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Vector2s) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Vector3s) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Vector4s) m_pShaderProgram->try_set_uniform(name, a);

	for (const auto& [name, a] : m_Integers)  m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Integer2s) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Integer3s) m_pShaderProgram->try_set_uniform(name, a);
	for (const auto& [name, a] : m_Integer4s) m_pShaderProgram->try_set_uniform(name, a);
}

webgl1es2_material::shader_ptr_type webgl1es2_material::getShaderProgram()
{
    return m_pShaderProgram;
}

void webgl1es2_material::setTexture(const std::string &aName, texture_ptr_type aValue)
{
    m_Textures[aName] = std::static_pointer_cast<webgl1es2_texture>(aValue);
}

void webgl1es2_material::setFloat(const std::string &aName, float aValue)
{
    m_Floats[aName] = aValue;
}

void webgl1es2_material::setVector2(const std::string &aName, graphics_vector2_type aValue)
{
    m_Vector2s[aName] = aValue;
}

void webgl1es2_material::setVector3(const std::string &aName, graphics_vector3_type aValue)
{
    m_Vector3s[aName] = aValue;
}

void webgl1es2_material::setVector4(const std::string &aName, graphics_vector4_type aValue)
{
    m_Vector4s[aName] = aValue;
}

void webgl1es2_material::setInteger(const std::string& aName, int aValue)
{
    m_Integers[aName] = aValue;
}
void webgl1es2_material::setInteger2(const std::string& aName, int aValue1, int aValue2)
{
    m_Integer2s[aName] = {aValue1, aValue2};
}
void webgl1es2_material::setInteger3(const std::string& aName, int aValue1, int aValue2, 
    int aValue3)
{
    m_Integer3s[aName] = {aValue1, aValue2, aValue3};
}

void webgl1es2_material::setInteger4(const std::string& aName, int aValue1, int aValue2, int aValue3, int aValue4)
{
    m_Integer4s[aName] = {aValue1, aValue2, aValue3, aValue4};
}

