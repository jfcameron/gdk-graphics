// © Joseph Cameron - All Rights Reserved

#include <string>
#include <gdk/graphics/glh.h>
#include <gdk/graphics/constraints.h>
#include <gdk/graphics/exception.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_texture.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/em_asm.h>
#endif

#if GDK_GRAPHICS_HAS_EGL
#define EGL_NO_X11
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#include <cmath>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <type_traits>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;


static inline webgl1es2_texture::format textureFormatToWebGL1ES2TextureFormat(const texture::format a) {
    switch(a) {
        case texture::format::grey: return webgl1es2_texture::format::alpha;
        case texture::format::rg: return webgl1es2_texture::format::luminance_alpha;
        case texture::format::rgb: return webgl1es2_texture::format::rgb;
        case texture::format::rgba: return webgl1es2_texture::format::rgba;
    }
    throw exception("unhandled format type");
}

static inline GLint webGL1ES2TextureFormatToToGLint(const webgl1es2_texture::format a) {
    switch(a) {
        case webgl1es2_texture::format::depth: {
#if defined JFC_TARGET_PLATFORM_Linux || defined JFC_TARGET_PLATFORM_Windows
            if (!GLEW_ARB_depth_texture) throw exception(
                "webgl1es2_texture: use of depth textures requires the "
                "availability of the depth_texture extention, which is "
                "not available on the current platform");
#endif
            return GL_DEPTH_COMPONENT;
        }
        case webgl1es2_texture::format::alpha: return GL_ALPHA;
        case webgl1es2_texture::format::luminance_alpha: return GL_LUMINANCE_ALPHA;
        case webgl1es2_texture::format::rgb: return GL_RGB;
        case webgl1es2_texture::format::rgba: return GL_RGBA;
    }
    throw exception("unhandled format type");
}

static inline GLint minification_filter_to_glint(const webgl1es2_texture::minification_filter a) {
    switch(a) {
        case webgl1es2_texture::minification_filter::linear: return GL_LINEAR;
        case webgl1es2_texture::minification_filter::nearest: return GL_NEAREST;
        case webgl1es2_texture::minification_filter::nearest_mipmap_nearest: return GL_NEAREST_MIPMAP_NEAREST;
        case webgl1es2_texture::minification_filter::linear_mipmap_nearest: return GL_LINEAR_MIPMAP_NEAREST;
        case webgl1es2_texture::minification_filter::nearest_mipmap_linear: return GL_NEAREST_MIPMAP_LINEAR;
        case webgl1es2_texture::minification_filter::linear_mipmap_linear: return GL_LINEAR_MIPMAP_LINEAR;
    }
    throw exception("unhandled minification filter");
}

static inline GLint magnification_filter_to_glint(const webgl1es2_texture::magnification_filter a) {
    switch(a) {
        case webgl1es2_texture::magnification_filter::linear: return GL_LINEAR;
        case webgl1es2_texture::magnification_filter::nearest: return GL_NEAREST;
    }
    throw exception("unhandled magnification filter");
}

static inline void unpack_rows_as_packed() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

static inline GLint wrap_mode_to_glint(const texture::wrap_mode a) {
    switch(a) {
        case texture::wrap_mode::clamped: return GL_CLAMP_TO_EDGE;
        case texture::wrap_mode::repeat: return GL_REPEAT;
        case texture::wrap_mode::mirrored: return GL_MIRRORED_REPEAT;
    }
    throw exception("unhandled wrap mode");
}

std::shared_ptr<webgl1es2_texture> webgl1es2_texture::make_checkerboard_of_death() {
    const std::vector<texture_data::channel_type> imageData({
        0x00, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0xff,
    });

    texture_data::view view;
    view.width = 2;
    view.height = 2;
    view.format = texture::format::rgba;
    view.data = &imageData.front();

    return std::make_shared<webgl1es2_texture>(view);
}

GLint webgl1es2_texture::getMaxTextureSize() {
    GLint max_texture_2d_size(0);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_2d_size);

    return max_texture_2d_size;
}

webgl1es2_texture::webgl1es2_texture(
    const texture_data::view &aTextureView,
    const wrap_mode aWrapModeU, 
    const wrap_mode aWrapModeV,
    const minification_filter minFilter,
    const magnification_filter magFilter)
: webgl1es2_texture(
    textureFormatToWebGL1ES2TextureFormat(aTextureView.format),
    aTextureView.width,
    aTextureView.height,
    aTextureView.data,
    aWrapModeU,
    aWrapModeV,
    minFilter,
    magFilter) 
{}

static inline bool samples_mipmaps(const webgl1es2_texture::minification_filter aFilter) {
    return aFilter != webgl1es2_texture::minification_filter::linear
        && aFilter != webgl1es2_texture::minification_filter::nearest;
}

static inline bool any_size_allowed(const texture::wrap_mode aU, const texture::wrap_mode aV,
    const bool aHasMipmaps) {
    return !aHasMipmaps && aU == texture::wrap_mode::clamped && aV == texture::wrap_mode::clamped;
}

static inline void validate_texture_size(const size_t aWidthInTexels, const size_t aHeightInTexels,
    const bool aAnySize) {
    if (!aAnySize && ((aWidthInTexels > 0 && !is_power_of_two(aWidthInTexels)) ||
        (aHeightInTexels > 0 && !is_power_of_two(aHeightInTexels))))
        throw exception("webgl1es2_texture dimensions must be a power of two unless the texture is "
            "clamped on both axes and its filter reads no mipmaps");

    if (const auto MAX_TEXTURE_2D_SIZE = webgl1es2_texture::getMaxTextureSize();
        aWidthInTexels > static_cast<std::size_t>(MAX_TEXTURE_2D_SIZE) ||
        aHeightInTexels > static_cast<std::size_t>(MAX_TEXTURE_2D_SIZE))
        throw exception(std::string("webgl1es2_texture too large for this platform. max: ")
            .append(std::to_string(MAX_TEXTURE_2D_SIZE)));
}

webgl1es2_texture::webgl1es2_texture(
    const webgl1es2_texture::format aFormat,
    const size_t aWidthInTexels,
    const size_t aHeightInTexels,
    const texture_data::channel_type *aData,
    const wrap_mode aWrapModeU,
    const wrap_mode aWrapModeV,
    const minification_filter minFilter,
    const magnification_filter magFilter)
: m_WrapU(aWrapModeU)
, m_WrapV(aWrapModeV)
, m_HasMipmaps(samples_mipmaps(minFilter))
, m_Handle([&]() {
    validate_texture_size(aWidthInTexels, aHeightInTexels,
        any_size_allowed(aWrapModeU, aWrapModeV, samples_mipmaps(minFilter)));

    GLuint handle;
    glGenTextures(1, &handle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, handle);

    auto format = webGL1ES2TextureFormatToToGLint(aFormat);

    unpack_rows_as_packed();
    glTexImage2D(GL_TEXTURE_2D, 
        0, 
        format,
        static_cast<GLsizei>(aWidthInTexels), 
        static_cast<GLsizei>(aHeightInTexels), 
        0, 
        format,
        GL_UNSIGNED_BYTE, 
        reinterpret_cast<const GLubyte *>(aData));

    if (samples_mipmaps(minFilter)) glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
        magnification_filter_to_glint(magFilter));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
        minification_filter_to_glint(minFilter));
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode_to_glint(aWrapModeU));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode_to_glint(aWrapModeV));

    return handle;
}(),
[](const GLuint handle) {
    glDeleteTextures(1, &handle);
})
, m_CurrentDataWidth(aWidthInTexels)
, m_CurrentDataHeight(aHeightInTexels)
, m_CurrentDataFormat(webGL1ES2TextureFormatToToGLint(aFormat))
{}

GLuint webgl1es2_texture::getHandle() const {
    return m_Handle.get();
}

void webgl1es2_texture::activateAndBind(const GLint aUnit) const {
    glActiveTexture(GL_TEXTURE0 + aUnit);
    glBindTexture(GL_TEXTURE_2D, m_Handle.get());
}

void webgl1es2_texture::hold_to(std::shared_ptr<const profile> pProfile) { m_pProfile = std::move(pProfile); }

void webgl1es2_texture::update_data(const texture_data::view &imageView) {
    const auto anySize = any_size_allowed(m_WrapU, m_WrapV, m_HasMipmaps);

    if (m_pProfile) check_texture(*m_pProfile, imageView.width, imageView.height, anySize);

    validate_texture_size(imageView.width, imageView.height, anySize);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Handle.get());

    auto format = webGL1ES2TextureFormatToToGLint(textureFormatToWebGL1ES2TextureFormat(imageView.format));

    unpack_rows_as_packed();
    glTexImage2D(GL_TEXTURE_2D, 
        0, 
        format,
        static_cast<GLsizei>(imageView.width), 
        static_cast<GLsizei>(imageView.height), 
        0, 
        format,
        GL_UNSIGNED_BYTE, 
        reinterpret_cast<const GLubyte *>(imageView.data));

    if (m_HasMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    m_CurrentDataFormat = format;
    m_CurrentDataWidth = imageView.width;
    m_CurrentDataHeight = imageView.height;
}

void webgl1es2_texture::update_data(const texture_data::view &imageView, 
    const size_t offsetX, const size_t offsetY) {
    if (offsetY + imageView.height > m_CurrentDataHeight ||
        offsetX + imageView.width > m_CurrentDataWidth)
        throw exception("webgl1es2_texture::update_data: "
            "when updating a subsection of texture data, the incoming "
            "data must be within the bounds of the existing data");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Handle.get());
    unpack_rows_as_packed();
    glTexSubImage2D(GL_TEXTURE_2D, 
        0, 
        static_cast<GLint>(offsetX),
        static_cast<GLint>(offsetY),
        static_cast<GLsizei>(imageView.width), 
        static_cast<GLsizei>(imageView.height), 
        m_CurrentDataFormat,
        GL_UNSIGNED_BYTE, 
        reinterpret_cast<const GLubyte *>(imageView.data));

    if (m_HasMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
}

void webgl1es2_texture::take_external_contents(const std::size_t aWidth, const std::size_t aHeight) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_WrapU = wrap_mode::clamped;
    m_WrapV = wrap_mode::clamped;
    m_HasMipmaps = false;
    m_CurrentDataFormat = GL_RGBA;
    m_CurrentDataWidth = aWidth;
    m_CurrentDataHeight = aHeight;
}

#ifdef __EMSCRIPTEN__
void webgl1es2_texture::update_data(const emscripten::val &aImageSource) {
    const auto dimension = [&aImageSource](const std::initializer_list<const char *> aNames) {
        for (const auto *const pName : aNames) {
            const auto value = aImageSource[pName];

            if (!value.isUndefined() && !value.isNull()) return value.as<std::size_t>();
        }

        throw exception("webgl1es2_texture::update_data: the image source has no size, so it is not a "
            "VideoFrame, an ImageBitmap, an image, a video or a canvas");
    };

    const auto width = dimension({"displayWidth", "width", "videoWidth"});
    const auto height = dimension({"displayHeight", "height", "videoHeight"});

    if (m_pProfile) check_texture(*m_pProfile, width, height, true);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Handle.get());

    EM_ASM({
        GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, Emval.toValue($0));
    }, aImageSource.as_handle());

    take_external_contents(width, height);
}
#endif

#if GDK_GRAPHICS_HAS_EGL
namespace {
    using egl_image_target_texture_2d_type = void (*)(GLenum, void *);

    [[nodiscard]] egl_image_target_texture_2d_type egl_image_target_texture_2d() {
        static const auto function = reinterpret_cast<egl_image_target_texture_2d_type>(
            eglGetProcAddress("glEGLImageTargetTexture2DOES"));

        return function;
    }
}

bool webgl1es2_texture::can_adopt_egl_images() {
    if (!egl_image_target_texture_2d()) return false;

    const auto *const pExtensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

    return pExtensions && std::strstr(pExtensions, "GL_OES_EGL_image");
}

void webgl1es2_texture::adopt(const egl_image_type apImage, const std::size_t aWidth, const std::size_t aHeight) {
    if (!apImage) throw exception("webgl1es2_texture::adopt: there is no image to adopt");

    if (!can_adopt_egl_images()) throw exception("webgl1es2_texture::adopt: this context has no "
        "GL_OES_EGL_image, so a texture cannot sample an EGLImage. The context must be EGL's, on a "
        "driver with the extension");

    if (m_pProfile) check_texture(*m_pProfile, aWidth, aHeight, true);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Handle.get());

    egl_image_target_texture_2d()(GL_TEXTURE_2D, apImage);

    take_external_contents(aWidth, aHeight);
}
#endif

bool webgl1es2_texture::operator==(const webgl1es2_texture &b) const { return m_Handle == b.m_Handle; }

