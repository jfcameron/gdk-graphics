// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VECTOR2UNIFORMCOLLECTION_H
#define GDK_GFX_VECTOR2UNIFORMCOLLECTION_H

#include <gdk/graphics_types.h>
#include <gdk/uniform_collection.h>

#include <iosfwd>
#include <memory>

namespace gdk
{
    /// \brief Manages and supplies Vector2 data for shaderprogram consumption
    class vector2_uniform_collection final : public uniform_collection<std::shared_ptr<graphics_vector2_type>>
    {
        friend std::ostream &operator<<(std::ostream &, const vector2_uniform_collection &);
      
    public:
        //! Uploads uniform data to currently used program
        void bind(const GLuint aProgramHandle) const override;

        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
            
        vector2_uniform_collection &operator=(const vector2_uniform_collection &) = delete;
        vector2_uniform_collection &operator=(vector2_uniform_collection &&) = delete;
      
        vector2_uniform_collection() = default;
        vector2_uniform_collection(const vector2_uniform_collection &) = default;
        vector2_uniform_collection(vector2_uniform_collection &&) = default;
        ~vector2_uniform_collection() = default;
    };

    std::ostream &operator<< (std::ostream &, const vector2_uniform_collection &);
}

#endif
