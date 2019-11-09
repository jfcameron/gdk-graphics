// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VECTOR3UNIFORMCOLLECTION_H
#define GDK_GFX_VECTOR3UNIFORMCOLLECTION_H

#include <gdk/uniform_collection.h>

#include <iosfwd>
#include <memory>

#include <gdk/graphics_types.h>

namespace gdk
{
    /// \brief Manages and supplies Vector3 data for shaderprogram consumption   
    class vector3_uniform_collection final : public uniform_collection<std::shared_ptr<graphics_vector3_type>>
    {
        friend std::ostream &operator<< (std::ostream &, const vector3_uniform_collection &);
      
    public:
        //! Uploads uniform data to currently used program
        void bind(const GLuint aProgramHandle) const override;
        
        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
            
        vector3_uniform_collection &operator=(const vector3_uniform_collection &) = delete;
        vector3_uniform_collection &operator=(vector3_uniform_collection &&) = delete;
            
        vector3_uniform_collection() = default;
        vector3_uniform_collection(const vector3_uniform_collection &) = default;
        vector3_uniform_collection(vector3_uniform_collection &&) = default;
        ~vector3_uniform_collection() = default;      
    };

    std::ostream &operator<< (std::ostream &, const vector3_uniform_collection &);
}

#endif
