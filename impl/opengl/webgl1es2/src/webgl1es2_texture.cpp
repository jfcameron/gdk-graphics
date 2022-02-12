// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/webgl1es2_texture.h>

#include <stb/stb_image.h>

#include <cmath>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <stdexcept>
#include <type_traits>
#include <vector>

using namespace gdk;

static constexpr char TAG[] = "texture";

static inline bool isPowerOfTwo(const long a)
{
    return std::ceil(std::log2(a)) == std::floor(std::log2(a));
}

static inline GLint textureFormatToGLint(const texture::data_format a)
{
    switch(a)
    {
        case texture::data_format::rgba: return GL_RGBA;
        case texture::data_format::rgb: return GL_RGB;
        //case texture::data_format::a: return GL_ALPHA;
        /*case texture::data_format::luminance_alpha: return GL_LUMINANCE_ALPHA;
        case texture::data_format::luminance: return GL_LUMINANCE;*/
        
        case texture::data_format::depth_component: 
        {
//TODO: deal with extensions on not glew platforms. Move this into a header.
#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
            if (!GLEW_ARB_depth_texture) throw std::runtime_error(
                "webgl1es2_texture: use of depth textures requires the "
                "availability of the depth_texture extention, which is "
                "not available on the current platform");
#endif

            return GL_DEPTH_COMPONENT;
        }
    }
    
    throw std::runtime_error("unhandled format type");
}

static inline GLint minification_filter_to_glint(const webgl1es2_texture::minification_filter a)
{
    switch(a)
    {
        case webgl1es2_texture::minification_filter::linear: return GL_LINEAR;
        case webgl1es2_texture::minification_filter::nearest: return GL_NEAREST;
        case webgl1es2_texture::minification_filter::nearest_mipmap_nearest: return GL_NEAREST_MIPMAP_NEAREST;
        case webgl1es2_texture::minification_filter::linear_mipmap_nearest: return GL_LINEAR_MIPMAP_NEAREST;
        case webgl1es2_texture::minification_filter::nearest_mipmap_linear: return GL_NEAREST_MIPMAP_LINEAR;
        case webgl1es2_texture::minification_filter::linear_mipmap_linear: return GL_LINEAR_MIPMAP_LINEAR;
    }
    
    throw std::runtime_error("unhandled minification filter");
}

static inline GLint magnification_filter_to_glint(const webgl1es2_texture::magnification_filter a)
{
    switch(a)
    {
        case webgl1es2_texture::magnification_filter::linear: return GL_LINEAR;
        case webgl1es2_texture::magnification_filter::nearest: return GL_NEAREST;
    }
    
    throw std::runtime_error("unhandled magnification filter");
}

static inline GLint wrap_mode_to_glint(const texture::wrap_mode a)
{
    switch(a)
    {
        case texture::wrap_mode::clamped: return GL_CLAMP_TO_EDGE;
        case texture::wrap_mode::repeat: return GL_REPEAT;
        case texture::wrap_mode::mirrored: return GL_MIRRORED_REPEAT;
    }
    
    throw std::runtime_error("unhandled wrap mode");
}

/*static inline GLint wrap_mode_to_glint(const webgl1es2_texture::wrap_mode a)
{
    switch(a)
    {
        case webgl1es2_texture::wrap_mode::clamp_to_edge: return GL_CLAMP_TO_EDGE;
        case webgl1es2_texture::wrap_mode::repeat: return GL_REPEAT;
        case webgl1es2_texture::wrap_mode::mirrored_repeat: return GL_MIRRORED_REPEAT;
    }
    
    throw std::runtime_error("unhandled wrap mode");
}*/

static inline GLenum bind_target_to_glenum(const webgl1es2_texture::bind_target a)
{
    switch(a)
    {
        case webgl1es2_texture::bind_target::texture_2d: return GL_TEXTURE_2D;
        case webgl1es2_texture::bind_target::cube_map: return GL_TEXTURE_CUBE_MAP;
    }

    throw std::runtime_error("unhandled bind target");
}

const std::shared_ptr<gdk::webgl1es2_texture> webgl1es2_texture::GetCheckerboardOfDeath()
{
    static std::once_flag initFlag;

    static std::shared_ptr<gdk::webgl1es2_texture> ptr;

    std::call_once(initFlag, []()
    {
        std::vector<GLubyte> webgl1es2_textureData(
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
        
        ptr = std::make_shared<gdk::webgl1es2_texture>(webgl1es2_texture::make_from_png_rgba32(webgl1es2_textureData));
    });
    
    return ptr;
};

webgl1es2_texture webgl1es2_texture::make_from_png_rgba32(const std::vector<std::underlying_type<std::byte>::type> awebgl1es2_textureData)
{
    //decode the png rgba32 data
    int width, height, components;

    if (std::unique_ptr<GLubyte, std::function<void(GLubyte *)>> decodedData(
        stbi_load_from_memory(&awebgl1es2_textureData[0]
            , static_cast<int>(awebgl1es2_textureData.size())
            , &width
            , &height
            , &components
            , STBI_rgb_alpha)
        , [](GLubyte *p)
        {
            stbi_image_free(p);
        }); decodedData)
    {
        image_data_2d_view data;
        data.width = width;
        data.height = height;
        data.format = data_format::rgba;
        data.data = reinterpret_cast<std::byte *>(decodedData.get());
        
        return webgl1es2_texture(data);
    }
    
    throw std::runtime_error(std::string(TAG).append(": could not decode RGBA32 data provided to webgl1es2_texture"));
}

static void verify_image_view(const image_data_2d_view &imageView)
{
    if (!isPowerOfTwo(imageView.width) || !isPowerOfTwo((imageView.height)))
        throw std::invalid_argument(std::string(TAG).append(
            ": webgl1es2_texture dimensions must be power of 2"));
    
    static std::once_flag once;
    static GLint max_texture_2d_size;

    std::call_once(once, []()
    {
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_2d_size);
    });

    if (imageView.width > max_texture_2d_size || 
        imageView.height > max_texture_2d_size)
        throw std::invalid_argument(std::string(TAG).append(
            ": webgl1es2_texture too large for this platform. max: " + 
            max_texture_2d_size));
}

webgl1es2_texture::webgl1es2_texture(const image_data_2d_view &imageView,
    const minification_filter minFilter,
    const magnification_filter magFilter,
    const wrap_mode wrapMode)
: m_BindTarget(bind_target_to_glenum(bind_target::texture_2d))    
, m_Handle([&]()
{
    verify_image_view(imageView);

    GLuint handle;

    glGenTextures(1, &handle);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(m_BindTarget, handle);

    auto format = textureFormatToGLint(imageView.format);

    glTexImage2D(m_BindTarget, 
        0, 
        format,
        imageView.width, 
        imageView.height, 
        0, 
        format,
        GL_UNSIGNED_BYTE, 
        const_cast<GLubyte *>(reinterpret_cast<const GLubyte *>(&imageView.data[0])));

    //Generate mip maps
    glGenerateMipmap(GL_TEXTURE_2D);

    //Select webgl1es2_texture filter functions
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
        magnification_filter_to_glint(magFilter));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
        minification_filter_to_glint(minFilter));
   
    //Set wrap modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode_to_glint(imageView.horizontal_wrap_mode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode_to_glint(imageView.vertical_wrap_mode));

    return handle;
}(),
[](const GLuint handle)
{
    glDeleteTextures(1, &handle);
})
, m_CurrentDataFormat(textureFormatToGLint(imageView.format))
, m_CurrentDataHeight(imageView.height)
, m_CurrentDataWidth(imageView.width)
{}

void webgl1es2_texture::update_data(const image_data_2d_view &imageView)
{
    verify_image_view(imageView);

    glActiveTexture(GL_TEXTURE0);

    const auto handle = getHandle();

    glBindTexture(m_BindTarget, handle);

    auto format = textureFormatToGLint(imageView.format);

    glTexImage2D(m_BindTarget, 
        0, 
        format,
        imageView.width, 
        imageView.height, 
        0, 
        format,
        GL_UNSIGNED_BYTE, 
        const_cast<GLubyte *>(reinterpret_cast<const GLubyte *>(&imageView.data[0])));

    glGenerateMipmap(GL_TEXTURE_2D);

    m_CurrentDataFormat = format;
    m_CurrentDataWidth = imageView.width;
    m_CurrentDataHeight = imageView.height;
}

void webgl1es2_texture::update_data(const image_data_2d_view &imageView, 
    const size_t offsetX, const size_t offsetY)
{
    if (offsetY + imageView.height > m_CurrentDataHeight ||
        offsetX + imageView.width > m_CurrentDataWidth)
        throw std::invalid_argument("webgl1es2_texture::update_data: "
            "when updating a subsection of texture data, the incoming "
            "data must be within the bounds of the existing data");

    if (textureFormatToGLint(imageView.format) != m_CurrentDataFormat)
        throw std::invalid_argument("webgl1es2_texture::update_data: "
            "when updating a subsection of texture data, the incoming data "
            "must match the existing data's format");

    glActiveTexture(GL_TEXTURE0);

    const auto handle = getHandle();

    glBindTexture(m_BindTarget, handle);

    glTexSubImage2D(m_BindTarget, 
        0, 
        offsetX,
        offsetY,
        imageView.width, 
        imageView.height, 
        m_CurrentDataFormat,
        GL_UNSIGNED_BYTE, 
        const_cast<GLubyte *>(reinterpret_cast<const GLubyte *>(&imageView.data[0])));

    glGenerateMipmap(GL_TEXTURE_2D);
}

bool webgl1es2_texture::operator==(const webgl1es2_texture &b) const { return m_Handle == b.m_Handle; }
bool webgl1es2_texture::operator!=(const webgl1es2_texture &b) const { return !(*this == b); }

GLuint webgl1es2_texture::getHandle() const
{
    return m_Handle.get();
}

