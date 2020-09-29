#include <gdk/webgl1es2_material.h>

#include <iostream>

using namespace gdk;

webgl1es2_material::webgl1es2_material(shader_ptr_type pShader)
	: m_pShaderProgram(pShader)
{}

void webgl1es2_material::activate()
{
	m_pShaderProgram->useProgram();

	for (const auto& [name, a] : m_Textures) m_pShaderProgram->setUniform(name, *a);

	for (const auto& [name, a] : m_Vector2s) m_pShaderProgram->setUniform(name, a);

	//TODO: activate the rest of this webgl1es2_material's uniforms...
}

webgl1es2_material::shader_ptr_type webgl1es2_material::getShaderProgram()
{
	return m_pShaderProgram;
}

void webgl1es2_material::setTexture(const std::string & aName, texture_ptr_type aValue)
{
    m_Textures[aName] = std::static_pointer_cast<webgl1es2_texture>(aValue);
}

void webgl1es2_material::setVector2(const std::string & aName, graphics_vector2_type aValue)
{
	m_Vector2s[aName] = aValue;
}

void webgl1es2_material::setFloat(const std::string& aName, float aValue)
{
	m_Floats[aName] = aValue;
}
