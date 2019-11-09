// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_FLOATUNIFORMCOLLECTION_H
#define GDK_GFX_FLOATUNIFORMCOLLECTION_H

#include <gdk/uniform_collection.h>

#include <iosfwd>
#include <memory>

namespace gdk
{
    /// \brief Manages and supplies float data for shaderprogram consumption
    class float_uniform_collection final : public uniform_collection<std::shared_ptr<float>>
    {
        friend std::ostream& operator<< (std::ostream &, const float_uniform_collection &);
      
    public:
        //! Uploads uniform data to currently used program
        void bind(const GLuint aProgramHandle) const override;

        //! zeroes uniform data in currently used program
        void unbind(const GLuint aProgramHandle) const override;
            
        float_uniform_collection &operator=(const float_uniform_collection &) = delete;
        float_uniform_collection &operator=(float_uniform_collection &&) = delete;
      
        float_uniform_collection() = default;
        float_uniform_collection(const float_uniform_collection &) = default;
        float_uniform_collection(float_uniform_collection &&) = default;
        ~float_uniform_collection() = default;
    };

    std::ostream &operator<< (std::ostream &, const float_uniform_collection &);
}

#endif
