// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

#include <gdk/opengl.h>
#include <jfc/lazy_ptr.h>

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
        friend std::ostream &operator <<(std::ostream &, const texture &);
        
        std::string m_Name; //!< Human friendly identifier
        GLuint m_Handle = {0};   //!< handle to the texture in the context
    
    public:
        /*enum ImageType
        {
            PNG_RGBA32,
            PNG_blahblah,
            JPG_blahblah
        }*/
        
        std::string getName() const;
        GLuint getHandle() const;
            
        texture &operator =(const texture &) = delete;
        texture &operator =(texture &&) = delete;
        
        texture(const std::string &aName, const std::vector<GLubyte> &aRGBA32PNGtextureData /* IMAGE_TYPE */ /*GLuint repeatmode = 0, GLuint magfilter = 0*/);
        texture() = delete;
        texture(const texture &) = delete;
        texture(texture &&);
        ~texture();

        static const jfc::lazy_ptr<gdk::texture> CheckeredTextureOfDeath; //!< texture for indicating texture related failure
    };

    std::ostream &operator<< (std::ostream &, const texture &);
}

#endif
