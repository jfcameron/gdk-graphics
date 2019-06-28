// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VECTOR2UNIFORMCOLLECTION_H
#define GDK_GFX_VECTOR2UNIFORMCOLLECTION_H

#include <gdk/graphics_types.h>
#include <gdk/uniformcollection.h>

#include <iosfwd>
#include <memory>

namespace gdk
{
    /// \brief Manages and supplies Vector2 data for shaderprogram consumption
    class Vector2UniformCollection final : public UniformCollection<std::shared_ptr<graphics_vector2_type>>
    {
        friend std::ostream &operator<<(std::ostream &, const Vector2UniformCollection &);
      
    public:
        //! Uploads uniform data to currently used program
        void bind(const GLuint aProgramHandle) const override;

        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
            
        Vector2UniformCollection &operator=(const Vector2UniformCollection &) = delete;
        Vector2UniformCollection &operator=(Vector2UniformCollection &&) = delete;
      
        Vector2UniformCollection() = default;
        Vector2UniformCollection(const Vector2UniformCollection &) = default;
        Vector2UniformCollection(Vector2UniformCollection &&) = default;
        ~Vector2UniformCollection() = default;
    };

    std::ostream &operator<< (std::ostream &, const Vector2UniformCollection &);
}

#endif
