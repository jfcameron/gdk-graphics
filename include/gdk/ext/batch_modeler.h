// © Joseph Cameron - All Rights Reserved

#ifndef GDK_BATCH_MODELER_H
#define GDK_BATCH_MODELER_H

#include <gdk/graphics_context.h>
#include <gdk/graphics_types.h>
#include <gdk/model_data.h>

namespace gdk {
    ///\brief a batch modeler assembles merges provided model data into a single set of data and then uploads it all to a single model
    /// typical use-cases of batching includes particles, sprites
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

