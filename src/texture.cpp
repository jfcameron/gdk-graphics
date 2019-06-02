// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/texture.h>

#include <stb/stb_image.h>

#include <nlohmann/json.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>

using namespace gdk;

static constexpr char TAG[] = "Texture";

const gdk::lazy_ptr<gdk::Texture> Texture::CheckeredTextureOfDeath([]()
{
    std::vector<GLubyte> textureData(
    {
        0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
        0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08,
        0x08, 0x02, 0x00, 0x00, 0x00, 0x4b, 0x6d, 0x29, 0xdc, 0x00, 0x00, 0x00,
        0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xae, 0xce, 0x1c, 0xe9, 0x00, 0x00,
        0x00, 0x04, 0x67, 0x41, 0x4d, 0x41, 0x00, 0x00, 0xb1, 0x8f, 0x0b, 0xfc,
        0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00,
        0x0e, 0xc3, 0x00, 0x00, 0x0e, 0xc3, 0x01, 0xc7, 0x6f, 0xa8, 0x64, 0x00,
        0x00, 0x00, 0x1b, 0x49, 0x44, 0x41, 0x54, 0x18, 0x57, 0x63, 0xf8, 0xff,
        0xff, 0xff, 0xcc, 0x9b, 0xaf, 0x30, 0x49, 0x06, 0xac, 0xa2, 0x40, 0x72,
        0x30, 0xea, 0xf8, 0xff, 0x1f, 0x00, 0xd3, 0x06, 0xab, 0x21, 0x92, 0xd9,
        0xa4, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
        0x60, 0x82
    });
    
    return new gdk::Texture("CheckerboardOfDeath", textureData);
});

std::ostream &gdk::operator<<(std::ostream &s, const Texture &a)
{
    return s << nlohmann::json
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },
        
        {"m_Name", a.m_Name},
        {"m_Handle", a.m_Handle},
    }
    .dump();
}

Texture::Texture(const std::string &aName, const std::vector<GLubyte> &aTextureData)
    : m_Name(aName)
{
    //decode the png rgba32 data
    int width, height, components;
    if (GLubyte *const decodedData = stbi_load_from_memory(&aTextureData[0], static_cast<int>(aTextureData.size()), &width, &height, &components, STBI_rgb_alpha)) //is STBI_default preferred?
    {
        //gdk::log(TAG, "CREATED: ", *this, " USING: ", "(name: ", aName, ", aTextureData.size:", aTextureData.size(), ")");
        
        //Copy the texture data to video memory
        glGenTextures(1, &m_Handle);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, decodedData);
    
        //Apply texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        //Cleanup
        stbi_image_free(decodedData);
    }
    else throw std::runtime_error(std::string(TAG).append("Could not decode RGBA32 data. Name: ").append(aName));
}

Texture::Texture(Texture &&other)
{
    m_Name   = std::move(other.m_Name);
    m_Handle = std::move(other.m_Handle);
    
    other.m_Handle = 0;
}

Texture::~Texture()
{
    if (m_Handle > 0) glDeleteTextures(1, &m_Handle);
}

std::string Texture::getName()const
{
    return m_Name;
}

GLuint Texture::getHandle()const
{
    return m_Handle;
}
