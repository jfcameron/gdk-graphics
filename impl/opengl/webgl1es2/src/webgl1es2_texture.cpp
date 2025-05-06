// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/graphics_exception.h>
#include <gdk/webgl1es2_texture.h>

#include <stb/stb_image.h>

#include <cmath>
#include <iostream>
#include <mutex>
#include <type_traits>
#include <vector>

using namespace gdk;

static constexpr char TAG[] = "texture";

static inline bool isPowerOfTwo(const long aNumber) {
    return aNumber > 0 && pow(2, static_cast<int>(log2(aNumber))) == aNumber;
}

static inline GLint textureFormatToGLint(const texture::format a) {
    switch(a) {
        case texture::format::depth: {
#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
            if (!GLEW_ARB_depth_texture) throw graphics_exception(
                "webgl1es2_texture: use of depth textures requires the "
                "availability of the depth_texture extention, which is "
                "not available on the current platform");
#endif
            return GL_DEPTH_COMPONENT;
        }
        case texture::format::grey: return GL_ALPHA;
        case texture::format::rgb: return GL_RGB;
        case texture::format::rgba: return GL_RGBA;
        default: break;
    }
    throw graphics_exception("unhandled format type");
}

static inline GLint minification_filter_to_glint(const webgl1es2_texture::minification_filter a) {
    switch(a) {
        case webgl1es2_texture::minification_filter::linear: return GL_LINEAR;
        case webgl1es2_texture::minification_filter::nearest: return GL_NEAREST;
        case webgl1es2_texture::minification_filter::nearest_mipmap_nearest: return GL_NEAREST_MIPMAP_NEAREST;
        case webgl1es2_texture::minification_filter::linear_mipmap_nearest: return GL_LINEAR_MIPMAP_NEAREST;
        case webgl1es2_texture::minification_filter::nearest_mipmap_linear: return GL_NEAREST_MIPMAP_LINEAR;
        case webgl1es2_texture::minification_filter::linear_mipmap_linear: return GL_LINEAR_MIPMAP_LINEAR;
        default: break;
    }
    throw graphics_exception("unhandled minification filter");
}

static inline GLint magnification_filter_to_glint(const webgl1es2_texture::magnification_filter a) {
    switch(a) {
        case webgl1es2_texture::magnification_filter::linear: return GL_LINEAR;
        case webgl1es2_texture::magnification_filter::nearest: return GL_NEAREST;
        default: break;
    }
    throw graphics_exception("unhandled magnification filter");
}

static inline GLint wrap_mode_to_glint(const texture::wrap_mode a) {
    switch(a) {
        case texture::wrap_mode::clamped: return GL_CLAMP_TO_EDGE;
        case texture::wrap_mode::repeat: return GL_REPEAT;
        case texture::wrap_mode::mirrored: return GL_MIRRORED_REPEAT;
        default: break;
    }
    throw graphics_exception("unhandled wrap mode");
}

const std::shared_ptr<gdk::webgl1es2_texture> webgl1es2_texture::GetCheckerboardOfDeath() {
    static std::shared_ptr<gdk::webgl1es2_texture> ptr;
    static std::once_flag initFlag;
    std::call_once(initFlag, []() {
        std::vector<std::underlying_type<std::byte>::type> imageData({
            0x00, 0xff, 0xff, 0xff,                                    
            0xff, 0xff, 0xff, 0xff,                                    
            0xff, 0xff, 0xff, 0xff,
            0x00, 0x00, 0x00, 0xff,
        });
        texture_data::view view;
        view.width = 2;
        view.height = 2;
        view.format = texture::format::rgba;
        view.data = reinterpret_cast<std::byte *>(&imageData.front());

        ptr = std::make_shared<gdk::webgl1es2_texture>(view);
    });
    
    return ptr;
};

const GLint webgl1es2_texture::getMaxTextureSize() {
    static std::once_flag once;
    static GLint max_texture_2d_size;

    std::call_once(once, []() {
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_2d_size);
    });

    return max_texture_2d_size;
}

static void verify_image_view(const texture_data::view &imageView) {
    if ((imageView.width > 0 && !isPowerOfTwo(imageView.width)) || 
        imageView.height > 0 && !isPowerOfTwo((imageView.height)))
        throw graphics_exception(std::string(TAG).append(
            ": webgl1es2_texture dimensions must be power of 2"));

    const auto MAX_TEXTURE_2D_SIZE(webgl1es2_texture::getMaxTextureSize());

    if (imageView.width > MAX_TEXTURE_2D_SIZE || 
        imageView.height > MAX_TEXTURE_2D_SIZE)
        throw graphics_exception(std::string(TAG).append(
            ": webgl1es2_texture too large for this platform. max: " + 
            MAX_TEXTURE_2D_SIZE));
}

webgl1es2_texture::webgl1es2_texture(const texture_data::view &imageView,
    const minification_filter minFilter,
    const magnification_filter magFilter,
    const wrap_mode wrapMode)
: m_Handle([&]() {
    verify_image_view(imageView);

    GLuint handle;
    glGenTextures(1, &handle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, handle);

    auto format = textureFormatToGLint(imageView.format);

    glTexImage2D(GL_TEXTURE_2D, 
        0, 
        format,
        imageView.width, 
        imageView.height, 
        0, 
        format,
        GL_UNSIGNED_BYTE, 
        const_cast<GLubyte *>(reinterpret_cast<const GLubyte *>(&imageView.data[0])));

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
        magnification_filter_to_glint(magFilter));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
        minification_filter_to_glint(minFilter));
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode_to_glint(imageView.horizontal_wrap_mode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode_to_glint(imageView.vertical_wrap_mode));

    return handle;
}(),
[](const GLuint handle) {
    glDeleteTextures(1, &handle);
})
, m_CurrentDataFormat(textureFormatToGLint(imageView.format))
, m_CurrentDataHeight(imageView.height)
, m_CurrentDataWidth(imageView.width)
{}

GLuint webgl1es2_texture::getHandle() const {
    return m_Handle.get();
}

void webgl1es2_texture::activateAndBind(const GLint aUnit) const {
    glActiveTexture(GL_TEXTURE0 + aUnit);
    glBindTexture(GL_TEXTURE_2D, m_Handle.get());
}

void webgl1es2_texture::update_data(const texture_data::view &imageView) {
    verify_image_view(imageView);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Handle.get());

    auto format = textureFormatToGLint(imageView.format);

    glTexImage2D(GL_TEXTURE_2D, 
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

void webgl1es2_texture::update_data(const texture_data::view &imageView, 
    const size_t offsetX, const size_t offsetY) {
    if (offsetY + imageView.height > m_CurrentDataHeight ||
        offsetX + imageView.width > m_CurrentDataWidth)
        throw graphics_exception("webgl1es2_texture::update_data: "
            "when updating a subsection of texture data, the incoming "
            "data must be within the bounds of the existing data");

    if (textureFormatToGLint(imageView.format) != m_CurrentDataFormat)
        throw graphics_exception("webgl1es2_texture::update_data: "
            "when updating a subsection of texture data, the incoming data "
            "must match the existing data's format");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Handle.get());
    glTexSubImage2D(GL_TEXTURE_2D, 
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

