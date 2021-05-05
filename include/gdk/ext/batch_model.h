// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_BATCH_MODEL_H
#define GDK_GFX_BATCH_MODEL_H

#include <memory>
#include <vector>

#include <gdk/vertex_data.h>
#include <gdk/graphics_types.h>

namespace gdk
{
    class model;
    namespace graphics { class context; }

    // \brief a batch model is a model whos vertex data is made up of
    /// the transformed vertex data of many separate objects.
    /// these objects could be separate instances of the same model or
    /// instances of many different models, the only restrictions are:
    /// the vertex formats must match
    /// they must all use the same material
    /// TODO: add a max_buffer_vertex_count, optional try_write_to_buffer, 
    /// optional points to the head of the data just added to the buffer, 
    /// so you can replace a section,
    /// returns nullptr if the vertex count would exceed the max
    class batch_model final
    {
    public:
        //! update buffer at location
        /// returns index into the buffer where the written data starts
        size_t write_to_buffer(size_t vertexDataIndex, 
            const graphics_vector3_type &aPos, 
            const graphics_vector3_type &aRot = {},
            const graphics_vector3_type &aScale = {1});

        /// \warn throws if the new data goes out of bounds of the buffer
        void rewrite_buffer_at(size_t BufferIndex, 
            size_t vertexDataIndex, 
            const graphics_vector3_type &pos,
            const graphics_vector3_type &rot,
            const graphics_vector3_type &sca = {1});

        //! gets a view to the buffer
        vertex_data::interleaved_data_view view_to_interleaved_data();

        //! erase all vertex data in the buffer
        void clear_buffer();

        //! clear the model's vertex data then upload the buffered data to the model
        void update_model();
        
        //! rewrite a subset of model's vertex data with the buffer range specified
        //void update_model(size_t aBeginIndex, size_t aEndIndex);

        //! throw is no a_Position
        //! throw is a_Postion does not have 3 components
        batch_model(std::shared_ptr<gdk::graphics::context> pContext, 
            std::vector<vertex_data> data);

        //! gets the model
        std::shared_ptr<gdk::model> model();

    private:
        //! model, the polygonal model rendered
        std::shared_ptr<gdk::model> m_pModel;

        //! vetex data buffer, modifed and uploaded to the model
        vertex_data m_Buffer = vertex_data({});

        //! input models.
        std::vector<vertex_data> m_Inputs;
    };
}

#endif

