// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/texture.h>

#include <stb/stb_image.h>

#include <cmath>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <stdexcept>
#include <vector>

using namespace gdk;

static constexpr char TAG[] = "texture";

bool static inline isPowerOfTwo(const long a)
{
    return std::ceil(std::log2(a)) == std::floor(std::log2(a));
}

const std::shared_ptr<gdk::texture> texture::GetCheckerboardOfDeath()
{
    static std::once_flag initFlag;

    static std::shared_ptr<gdk::texture> ptr;

    std::call_once(initFlag, []()
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
        
        ptr = std::make_shared<gdk::texture>(texture::make_from_png_rgba32(textureData));
    });
    
    return ptr;
};

texture texture::make_from_png_rgba32(const std::vector<GLubyte> atextureData)
{
    //decode the png rgba32 data
    int width, height, components;

    if (std::unique_ptr<GLubyte, std::function<void(GLubyte *)>> decodedData(
        stbi_load_from_memory(&atextureData[0]
            , static_cast<int>(atextureData.size())
            , &width
            , &height
            , &components
            , STBI_rgb_alpha)
        , [](GLubyte *p)
        {
            stbi_image_free(p);
        }); decodedData)
    {
        return texture(decodedData.get(), width, height);
    }
    
    throw std::runtime_error(std::string(TAG).append(": could not decode RGBA32 data provided to texture"));
}

texture::texture(GLubyte *const pDecodedImageData, const long width, const long height)
: m_Handle([&]()
{
    if (!isPowerOfTwo(width) || !isPowerOfTwo(height)) 
        throw std::invalid_argument(std::string(TAG).append(": texture dimensions must be power of 2"));

    GLuint handle;
        
    //Copy the texture data to video memory
    glGenTextures(1, &handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pDecodedImageData);

    //Apply texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return handle;
}(),
[](const GLuint handle)
{
    glDeleteTextures(1, &handle);
})
{}

GLuint texture::getHandle() const
{
    return m_Handle.get();
}

