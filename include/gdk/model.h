// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <gdk/vertex_data.h>

namespace gdk {
    /// \brief Vertex data representing a 3D graphical object
    class model {
    public:
        enum class usage_hint {
            dynamic,   //!< data will be rewritten 
            streaming, //!< data will be rewritten extremely frequently
            write_once //!< data will only be written once
        };

        /// \brief replace vertex data held by this model
        virtual void upload_vertex_data(const usage_hint &, const vertex_data &vertexDataView) = 0;

        /// \brief upload a section of vertex data
        //virtual void upload_vertex_data(const usage_hint &, const vertex_data &vertexDataView, const size_t offset) = 0;

        virtual ~model() = default;

    protected:
        model() = default;
    };
}

#endif

