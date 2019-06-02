// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTUREUNIFORMCOLLECTION_H
#define GDK_GFX_TEXTUREUNIFORMCOLLECTION_H

#include <gdk/uniformcollection.h>
#include <gdk/texture.h>
#include <gdk/default_ptr.h>

#include <iosfwd>

namespace gdk
{
    /// \brief collection of textures to be uploaded to the shader stages at draw time
    class TextureUniformCollection final : public UniformCollection<default_ptr<Texture>>
    {
        friend std::ostream &operator<< (std::ostream &, const TextureUniformCollection &);
            
    public:
        //! zeroes uniform data in currently used program
        void bind(const GLuint aProgramHandle) const override;

        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
      
        TextureUniformCollection &operator=(const TextureUniformCollection &) = delete;
        TextureUniformCollection &operator=(TextureUniformCollection &&) = delete;
           
        TextureUniformCollection() = default;
        TextureUniformCollection(const TextureUniformCollection &) = default;
        TextureUniformCollection(TextureUniformCollection &&) = default;
        ~TextureUniformCollection() = default;
    };

    std::ostream &operator<< (std::ostream &, const TextureUniformCollection &);
}

#endif
