#include <gdk/webgl1es2_material.h>

#include <iostream>

using namespace gdk;

void webgl1es2_material::setTexture(const std::string &aTextureName, texture_ptr_type aTexture)
{
    m_Textures[aTextureName] = std::static_pointer_cast<webgl1es2_texture>(aTexture);
}

void webgl1es2_material::setVector2(const std::string &aVector2Name, graphics_vector2_type aVector2)
{
	m_Vector2s[aVector2Name] = aVector2;
}

void webgl1es2_material::activate()
{
    m_pShaderProgram->useProgram();

    for (const auto &[name, a] : m_Textures) m_pShaderProgram->setUniform(name, *a);

	for (const auto &[name, a] : m_Vector2s) m_pShaderProgram->setUniform(name, a);

    //TODO: activate the rest of this webgl1es2_material's uniforms...
}

webgl1es2_material::webgl1es2_material(shader_ptr_type pShader)
: m_pShaderProgram(pShader)
{}

webgl1es2_material::shader_ptr_type webgl1es2_material::getShaderProgram()
{
    return m_pShaderProgram;
}

