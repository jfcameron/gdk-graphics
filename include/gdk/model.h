// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <string>
#include <unordered_map>

#include <gdk/graphics_context.h>
#include <gdk/vertex_data_view.h>

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    class model
    {
    public:
        /// \brief replace vertex data held by this model
        virtual void update_vertex_data(const vertex_data_view &vertexDataView) = 0;

        //TODO: virtual void update_vertex_data(data, range or index) = 0;

        virtual ~model() = default;

    protected:
        model() = default;
    };
}

#endif

