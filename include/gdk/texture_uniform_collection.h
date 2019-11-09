// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTUREUNIFORMCOLLECTION_H
#define GDK_GFX_TEXTUREUNIFORMCOLLECTION_H

#include <jfc/default_ptr.h>
#include <gdk/texture.h>
#include <gdk/uniform_collection.h>

#include <iosfwd>

namespace gdk
{
    /// \brief collection of textures to be uploaded to the shader stages at draw time
    class texture_uniform_collection final : public uniform_collection<jfc::default_ptr<texture>>
    {
        friend std::ostream &operator<<(std::ostream &, const texture_uniform_collection &);
            
    public:
        //! zeroes uniform data in currently used program
        void bind(const GLuint aProgramHandle) const override;

        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
      
        texture_uniform_collection &operator=(const texture_uniform_collection &) = delete;
        texture_uniform_collection &operator=(texture_uniform_collection &&) = delete;
           
        texture_uniform_collection() = default;
        texture_uniform_collection(const texture_uniform_collection &) = default;
        texture_uniform_collection(texture_uniform_collection &&) = default;
        ~texture_uniform_collection() = default;
    };

    std::ostream &operator<< (std::ostream &, const texture_uniform_collection &);
}

#endif

