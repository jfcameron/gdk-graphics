// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VECTOR4UNIFORMCOLLECTION_H
#define GDK_GFX_VECTOR4UNIFORMCOLLECTION_H

#include <gdk/graphics_types.h>
#include <gdk/uniform_collection.h>

#include <iosfwd>
#include <memory>

namespace gdk
{
    /// \brief Set of Vector4s to upload to the shaderprogram currently in use by the gl context
    class vector4_uniform_collection final : public uniform_collection<std::shared_ptr<graphics_vector4_type>>
    {
        friend std::ostream &operator<< (std::ostream &, const vector4_uniform_collection &);
        
    public:
        //! Uploads uniform data to currently used program
        void bind(const GLuint aProgramHandle) const override;

        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
            
        vector4_uniform_collection &operator=(const vector4_uniform_collection &) = delete;
        vector4_uniform_collection &operator=(vector4_uniform_collection &&) = delete;
            
        vector4_uniform_collection() = default;
        vector4_uniform_collection(const vector4_uniform_collection &) = default;
        vector4_uniform_collection(vector4_uniform_collection &&) = default;
        ~vector4_uniform_collection() = default;
    };

    std::ostream &operator<< (std::ostream &, const vector4_uniform_collection &);
}

#endif
