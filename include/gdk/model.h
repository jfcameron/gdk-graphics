// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <gdk/model_data.h>

namespace gdk {
    /// \brief Vertex data representing a 3D graphical object
    class model {
    public:
        enum class usage_hint {
            dynamic,   //!< data will be rewritten 
            streaming, //!< data will be rewritten extremely frequently
            upload_once_ //!< data will only be written once
        };

        /// \brief replace vertex data held by this model
        virtual void upload(const usage_hint &, const model_data &vertexDataView) = 0;

        /// \brief upload a section of vertex data
        //virtual void upload(const usage_hint &, const model_data &vertexDataView, const size_t offset) = 0;

        virtual ~model() = default;

    protected:
        model() = default;
    };
}

#endif

