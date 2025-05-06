// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <gdk/vertex_data.h>

namespace gdk {
    /// \brief Vertex data representing a 3D graphical object
    class model {
    public:
        enum class UsageHint {
            Static,
            Dynamic,
            Streaming
        };

        /// \brief replace vertex data held by this model
        virtual void update_vertex_data(const UsageHint &, const vertex_data &vertexDataView) = 0;

        //TODO: virtual void update_vertex_data(data, range or index) = 0;

        virtual ~model() = default;

    protected:
        model() = default;
    };
}

#endif

