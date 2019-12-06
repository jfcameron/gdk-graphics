// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MODEL_H
#define GDK_GFX_MODEL_H

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    class model
    {
    public:
        /// \brief raw vertex data used to generate the model
        /// \warn views do not own their data; it is the user's responsibility to make sure the data is valid until the model is generated and to clean up afterwards.
        //TODO how to handle multiple vbos? hmm
        struct vertex_data_view
        {   
            //TODO: implement
        };

        virtual ~model() = default;

    protected:
        model() = default;
    };
}

#endif
