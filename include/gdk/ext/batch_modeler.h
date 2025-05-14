// © Joseph Cameron - All Rights Reserved

#ifndef GDK_BATCH_MODELER_H
#define GDK_BATCH_MODELER_H

#include <gdk/graphics_context.h>
#include <gdk/graphics_types.h>
#include <gdk/model_data.h>

namespace gdk {
    ///\brief a batch modeler assembles a single model out of a list of different model data.
    /// batching is typically used to put together a large number of low vertex-count data into a single model with a more
    /// reasonable vertex count, like in the case of particles
    class batch_modeler final {
    public:
        //! get the model, contains the uploaded model data for the batch
        const_model_ptr_type model() const;

        //! get the material, used to render the model
        const_material_ptr_type material() const;

        //! get the number of vertices in the current buffer
        size_t vertex_count() const;

        //! add model data to the batch
        void push_back(const model_data &data);

        //! upload the batch data to the model and clear the batch
        void upload();

        batch_modeler(context_ptr_type aGraphics, material_ptr_type aMaterial);

    private:
        material_ptr_type m_pMaterial;
        model_data m_BatchData = {};
        model_ptr_type m_pModel; 
    };
}

#endif

