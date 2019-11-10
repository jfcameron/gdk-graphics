// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

#include <gdk/opengl.h>
#include <jfc/lazy_ptr.h>
#include <jfc/unique_handle.h>

#include <iosfwd>
#include <string>
#include <vector>

namespace gdk
{
    /// \brief texture represents an image. RGBA32, 2D.
    ///
    /// \todo Either throw on non power 2 textures OR support them (by padding or cropping)
    class texture final
    {
        //GLuint m_Handle = {0};   //!< handle to the texture in the context
        jfc::unique_handle<GLuint> m_Handle;
    
    public:
        /*enum ImageType
        {
            PNG_RGBA32,
            PNG_blahblah,
            JPG_blahblah
        }*/

        /// \brief returns the handle to the texture in the opengl context
        GLuint getHandle() const;

        /// \brief move semantics
        texture &operator=(texture &&) = default;
        /// \brief move semantics
        texture(texture &&) = default;
        
        texture(const std::vector<GLubyte> &aRGBA32PNGtextureData /* IMAGE_TYPE */ /*GLuint repeatmode = 0, GLuint magfilter = 0*/);

        /// \brief texture for indicating texture related failure
        static const jfc::lazy_ptr<gdk::texture> CheckeredTextureOfDeath; 
    };
}

#endif
