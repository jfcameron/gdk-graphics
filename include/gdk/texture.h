// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

#include <gdk/opengl.h>
#include <jfc/unique_handle.h>

#include <iosfwd>
#include <string>
#include <vector>

namespace gdk
{
    /// \brief a texture generally represents the color of a surface
    class texture final
    {
        jfc::unique_handle<GLuint> m_Handle;
    
    public:
        /// \brief returns the handle to the texture in the opengl context
        GLuint getHandle() const;

        /// \brief move semantics
        texture &operator=(texture &&) = default;
        /// \brief move semantics
        texture(texture &&) = default;
       
        //texture(const std::vector<GLubyte> &aRGBA32PNGtextureData /* IMAGE_TYPE */ /*GLuint repeatmode = 0, GLuint magfilter = 0*/);

        /// \brief creates a texture from decoded image data.
        texture(GLubyte *const pDecodedImageData, const long width, const long height);

        /// \brief texture useful for indicating texture related failure
        /// lazily instantiated.
        static const std::shared_ptr<gdk::texture> GetCheckerboardOfDeath(); 

        /// \brief constructs a texture from png rgba32 file data
        /// \throws invalid_argument if the image could not be decoded (badly formed or not a PNG with component format RGBA32, etc.)
        static texture make_from_png_rgba32(const std::vector<GLubyte> aRGBA32PNGData);
    };
}

#endif
