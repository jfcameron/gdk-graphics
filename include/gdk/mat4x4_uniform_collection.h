// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MAT4X4UNIFORMCOLLECTION_H
#define GDK_GFX_MAT4X4UNIFORMCOLLECTION_H

#include <gdk/graphics_types.h>
#include <gdk/mat4x4.h>
#include <gdk/uniform_collection.h>

#include <iosfwd>

namespace gdk
{
    /// \brief Set of Mat4x4s to upload to the shaderprogram currently in use by the gl context
    class mat4x4_uniform_collection final : public gdk::uniform_collection<graphics_mat4x4_type>
    {
        friend std::ostream& operator<< (std::ostream&, const mat4x4_uniform_collection&);
      
    public:
        //! Uploads uniform data to currently used program
        void bind(const GLuint aProgramHandle) const override;
        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
      
        mat4x4_uniform_collection& operator=(const mat4x4_uniform_collection&) = delete;
      
        mat4x4_uniform_collection() = default;
        mat4x4_uniform_collection(const mat4x4_uniform_collection&) = default;
        mat4x4_uniform_collection(mat4x4_uniform_collection&&) = default;
        ~mat4x4_uniform_collection() = default;
    };

    std::ostream &operator<< (std::ostream &, const mat4x4_uniform_collection &);
}

#endif
