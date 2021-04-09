// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/webgl1es2_texture.h>
#include <gdk/webgl1es2_texture_camera.h>

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>

using namespace gdk;

static inline bool isPowerOfTwo(const long a)
{
    return std::ceil(std::log2(a)) == std::floor(std::log2(a));
}

webgl1es2_texture_camera::webgl1es2_texture_camera()
: webgl1es2_camera()
, m_FrameBufferHandle([]()
{
    GLuint handle;
    glGenFramebuffers(1, &handle);

    return handle;
}(), 
[](GLuint handle)
{
    glDeleteFramebuffers(1, &handle);
})
, m_ColorBufferTextureHandle([]()
{
    

    return 0;
}(),
[](GLuint handle)
{

})
{
    //glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
}

void webgl1es2_texture_camera::set_perspective_projection(const float aFieldOfView, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio) 
{
    webgl1es2_camera::set_perspective_projection(aFieldOfView, 
        aNearClippingPlane, 
        aFarClippingPlane,  
        aViewportAspectRatio);
}

void webgl1es2_texture_camera::set_orthographic_projection(const float aWidth, const float aHeight, const float aNearClippingPlane, const float aFarClippingPlane, const float aViewportAspectRatio) 
{
    webgl1es2_camera::set_orthographic_projection(aWidth, 
        aHeight, 
        aNearClippingPlane, 
        aFarClippingPlane, 
        aViewportAspectRatio);
}

void webgl1es2_texture_camera::set_viewport(const float aX, const float aY, const float aWidth, const float aHeight) 
{
    webgl1es2_camera::set_viewport(aX, aY, aWidth, aHeight);
}

void webgl1es2_texture_camera::set_clear_color(const gdk::color& acolor) 
{
    webgl1es2_camera::set_clear_color(acolor);
}

void webgl1es2_texture_camera::set_clear_mode(const clear_mode aClearMode) 
{
    webgl1es2_camera::set_clear_mode(aClearMode); 
}

void webgl1es2_texture_camera::set_view_matrix(const gdk::graphics_vector3_type& aWorldPos, const gdk::graphics_quaternion_type& aRotation) 
{
    webgl1es2_camera::set_view_matrix(aWorldPos, aRotation); 
}

