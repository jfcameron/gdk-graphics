#include <gdk/webgl1es2_material.h>

using namespace gdk;

void webgl1es2_material::setTexture(const std::string &aTextureName, texture_ptr_type aTexture)
{
    m_Textures[aTextureName] = aTexture;
}

void webgl1es2_material::activate()
{
    m_pShader->useProgram();

    for (const auto &[name, texture] : m_Textures)
    {
        m_pShader->setUniform(name, *texture);
    }

    //TODO: activate the rest of this webgl1es2_material's uniforms...
}

webgl1es2_material::webgl1es2_material(shader_ptr_type pShader)
: m_pShader(pShader)
{}
