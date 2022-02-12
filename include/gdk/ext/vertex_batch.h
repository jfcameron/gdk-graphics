// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_BATCH_MODELER_H
#define GDK_GFX_BATCH_MODELER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include <set>

#include <gdk/vertex_data.h>
#include <gdk/graphics_types.h>

#include <gdk/graphics_context.h>

// TODO: This is a BAD abstraction! Once vertex_data operator+ and vertex_data::overwrite are implemented, remove this file & .cpp

namespace gdk
{
    class model;
    namespace graphics { class context; }

    // TODO: I am no longer sure this is a good abstraction at all, even the old version.
    // what if e.g the text_modeler emitted vertex_data? then the user can easily batch the data
    // like pModel->update_data(menuText.vertex_data() + menuBackground.vertex_data());
    // this avoids the "instancing" concept. still pay to concat but that is not avoidable.
    // TODO vertex_data support + semantics
    // I think the above is correct. clean up the vertex_data class. can throw this one away.

    // \brief used to create a complex model out of many sets of vertex data of the same format
    ///
    /// This abstraction is needed to efficently render many instances of simple models. e.g:
    /// Particles, text, terrain, voxels. Combining very low vert count shapes together into more complex ones
    /// allows better throughput on graphics cards, which are designed to parallelize per vertex work
    ///
    class vertex_batch final 
    {
    public:
        /// \brief a handle to an instance of vertex data
        using instance_handle_type = size_t;

        /// \brief creates a new, empty instance, returns a handle to it
        instance_handle_type add_instance();

        /// \brief adds a new instance of vertex data, returns a handle to it
        instance_handle_type add_instance(const vertex_data &aVertexData);

        /// \brief overwrites an instance of vertex data stored at the handle or adds a new one if it doesnt exist
        ///
        /// handle is provided as a mutable reference, on add, the value of aHandle will be rewritten
        void overwrite_or_add_instance(instance_handle_type &aHandle, const vertex_data &aVertexData);

        /// \brief removes the vertex data instance at the handle
        /// \note no effect if no instance exists at the handle
        void remove_instance(const instance_handle_type aHandle);

        /// \brief concatenates all instanced data into a single vertex data object, uploads that to the model
        void update_model();

        /// \brief clears all buffers
        void clear();
       
        //! gets the batched model
        std::shared_ptr<gdk::model> model() const;
        
        //! constructs a vertex_batcher
        vertex_batch(std::shared_ptr<gdk::graphics::context> pContext);

    private:
        //! handles that have been removed by the user
        std::set<instance_handle_type> m_recycled_instance_handles;

        //! used to create new instance handles
        instance_handle_type m_handle_counter = 0;

        //! collection of individual vertex data sets
        std::map<instance_handle_type, vertex_data> m_vertex_data_instances;

        //! vetex data buffer, modifed and uploaded to the model
        vertex_data m_VertexDataBuffer = vertex_data({});

        //! the final polygonal model
        std::shared_ptr<gdk::model> m_pModel;
    };
}

#endif

