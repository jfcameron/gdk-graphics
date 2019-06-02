// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

#include <gdk/lazy_ptr.h>
#include <gdk/opengl.h>

#include <iosfwd>
#include <string>
#include <vector>

namespace gdk
{
    /// \brief Texture represents an image. RGBA32, 2D.
    ///
    /// \todo Either throw on non power 2 textures OR support them (by padding or cropping)
    class Texture final
    {
        friend std::ostream &operator <<(std::ostream &, const Texture &);
        
        std::string m_Name = {}; //!< Human friendly identifier
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
            
        Texture &operator =(const Texture &) = delete;
        Texture &operator =(Texture &&) = delete;
        
        Texture(const std::string &aName, const std::vector<GLubyte> &aRGBA32PNGTextureData /* IMAGE_TYPE */ /*GLuint repeatmode = 0, GLuint magfilter = 0*/);
        Texture() = delete;
        Texture(const Texture &) = delete;
        Texture(Texture &&);
        ~Texture();

        static const gdk::lazy_ptr<gdk::Texture> CheckeredTextureOfDeath; //!< texture for indicating texture related failure
    };

    std::ostream &operator<< (std::ostream &, const Texture &);
}

#endif
