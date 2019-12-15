// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

#include <string>
#include <unordered_map>

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    class model
    {
    public:

        virtual ~model() = default;

    protected:
        //virtual model(const vertex_data_view &vertexDataView){}

        //TODO: delete
        model() = default;// delete;//default;
    };
}

#endif
