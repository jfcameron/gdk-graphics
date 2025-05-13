// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/graphics_exception.h>
#include <gdk/texture_data.h>
#include <gdk/webgl1es2_texture.h>
#include <gdk/webgl1es2_texture_camera.h>

#include <iostream>
#include <mutex>
#include <sstream>

using namespace gdk;

webgl1es2_texture_camera::webgl1es2_texture_camera(const graphics_intvector2_type &aTextureSize)
: webgl1es2_camera()
, m_TextureSize(aTextureSize)
, m_FrameBufferHandle([]() {
    GLuint handle; 
    glGenFramebuffers(1, &handle);
    
    return handle;
}(), 
[](GLuint handle) {
    glDeleteFramebuffers(1, &handle);
})
, m_DepthBuffer([&]() {
    GLuint handle; 
    glGenRenderbuffers(1, &handle);

    glBindRenderbuffer(GL_RENDERBUFFER, handle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
        static_cast<size_t>(aTextureSize.x), static_cast<size_t>(aTextureSize.y));

    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferHandle.get());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, handle);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return handle;
}(),
[](GLuint handle) {
    glDeleteRenderbuffers(1, &handle);
})
{}

void webgl1es2_texture_camera::set_perspective_projection(const float aFieldOfView, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio) {
    webgl1es2_camera::set_perspective_projection(aFieldOfView, 
        aNearClippingPlane, 
        aFarClippingPlane,  
        aViewportAspectRatio);
}

void webgl1es2_texture_camera::set_orthographic_projection(const float aWidth, const float aHeight, const float aNearClippingPlane, const float aFarClippingPlane, 
    const float aViewportAspectRatio) {
    webgl1es2_camera::set_orthographic_projection(aWidth, 
        aHeight, 
        aNearClippingPlane, 
        aFarClippingPlane, 
        aViewportAspectRatio);
}

void webgl1es2_texture_camera::set_clear_color(const gdk::color& acolor) {
    webgl1es2_camera::set_clear_color(acolor);
}

void webgl1es2_texture_camera::set_clear_mode(const clear_mode aClearMode) {
    webgl1es2_camera::set_clear_mode(aClearMode); 
}

void webgl1es2_texture_camera::set_transform(const gdk::graphics_vector3_type& aWorldPos, const gdk::graphics_quaternion_type& aRotation) {
    webgl1es2_camera::set_transform(aWorldPos, aRotation); 
}

void webgl1es2_texture_camera::set_transform(const gdk::graphics_mat4x4_type &aMatrix) {
    webgl1es2_camera::set_transform(aMatrix);
}

const std::shared_ptr<gdk::texture> webgl1es2_texture_camera::get_color_texture(const size_t i) const {
    if (i) throw graphics_exception("webgl1es2_texture_camera: "
        "only supports a single texture bound to the color "
        "buffer. this is a limitation with the standard");

    if (!m_ColorBufferTexture.has_value()) m_ColorBufferTexture = {[&]() {
        auto p = new webgl1es2_texture({
                static_cast<size_t>(m_TextureSize.x), 
                static_cast<size_t>(m_TextureSize.y), 
                texture::format::rgb, 
                nullptr
            },
            texture::wrap_mode::repeat,
            texture::wrap_mode::repeat,
            webgl1es2_texture::minification_filter::nearest,
            webgl1es2_texture::magnification_filter::nearest);

        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferHandle.get());

        glFramebufferTexture2D(GL_FRAMEBUFFER, 
            GL_COLOR_ATTACHMENT0, 
            GL_TEXTURE_2D,
            p->getHandle(), 
            0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw graphics_exception("webgl1es2_texture_camera: "
                "invalid framebuffer during color attachment");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return std::shared_ptr<webgl1es2_texture>(p);
    }()};

    return m_ColorBufferTexture.value();
}

const std::shared_ptr<gdk::texture> webgl1es2_texture_camera::get_depth_texture() const {
    if (!m_DepthBufferTexture.has_value()) m_DepthBufferTexture = {[&]() {
        auto p = new webgl1es2_texture(
            webgl1es2_texture::format::depth, 
            static_cast<size_t>(m_TextureSize.x), 
            static_cast<size_t>(m_TextureSize.y), 
            nullptr,
            texture::wrap_mode::clamped,
            texture::wrap_mode::clamped,
            webgl1es2_texture::minification_filter::nearest,
            webgl1es2_texture::magnification_filter::nearest);

        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferHandle.get());

        glFramebufferTexture2D(GL_FRAMEBUFFER, 
            GL_DEPTH_ATTACHMENT, 
            GL_TEXTURE_2D,
            p->getHandle(), 
            0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw graphics_exception("webgl1es2_texture_camera: "
                "invalid framebuffer during depth attachment");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return std::shared_ptr<webgl1es2_texture>(p);
    }()};

    return m_DepthBufferTexture.value();
}

void webgl1es2_texture_camera::activate() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferHandle.get());

    glh::Viewport({0, 0}, m_TextureSize);
    glh::Scissor ({0, 0}, m_TextureSize);

    webgl1es2_camera::activate_clear_mode();
}

