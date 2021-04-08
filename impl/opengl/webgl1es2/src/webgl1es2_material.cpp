// © Joseph Cameron - All Rights Reserved

#include <gdk/webgl1es2_material.h>

#include <iostream>

using namespace gdk;

webgl1es2_material::webgl1es2_material(shader_ptr_type pShader)
    : m_pShaderProgram(pShader)
{}

void webgl1es2_material::activate()
{
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

