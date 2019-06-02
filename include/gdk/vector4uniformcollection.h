// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VECTOR4UNIFORMCOLLECTION_H
#define GDK_GFX_VECTOR4UNIFORMCOLLECTION_H

#include <gdk/uniformcollection.h>

#include <iosfwd>
#include <memory>

namespace gdk
{
    struct Vector4;
    
    /// \brief Set of Vector4s to upload to the shaderprogram currently in use by the gl context
    class Vector4UniformCollection final : public UniformCollection<std::shared_ptr<Vector4>>
    {
        friend std::ostream &operator<< (std::ostream &, const Vector4UniformCollection &);
        
    public:
        //! Uploads uniform data to currently used program
        void bind(const GLuint aProgramHandle) const override;

        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
            
        Vector4UniformCollection &operator=(const Vector4UniformCollection &) = delete;
        Vector4UniformCollection &operator=(Vector4UniformCollection &&) = delete;
            
        Vector4UniformCollection() = default;
        Vector4UniformCollection(const Vector4UniformCollection &) = default;
        Vector4UniformCollection(Vector4UniformCollection &&) = default;
        ~Vector4UniformCollection() = default;
    };

    std::ostream &operator<< (std::ostream &, const Vector4UniformCollection &);
}

#endif
