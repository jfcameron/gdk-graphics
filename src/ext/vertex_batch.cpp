// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics_context.h>
#include <gdk/ext/vertex_batch.h>

using namespace gdk;

vertex_batch::vertex_batch(std::shared_ptr<gdk::graphics::context> pContext)
: m_pModel(pContext->make_model())
{}

std::shared_ptr<gdk::model> vertex_batch::model() const
{
    return m_pModel;
}

vertex_batch::instance_handle_type vertex_batch::add_instance()
{
    return add_instance({});
}

vertex_batch::instance_handle_type vertex_batch::add_instance(const vertex_data &aVertexData)
{
    m_vertex_data_instances.insert_or_assign(m_handle_counter, aVertexData);

    if (m_recycled_instance_handles.empty())
    {
        return m_handle_counter++;
    }
        
    const auto handle(*m_recycled_instance_handles.begin());
    
    m_recycled_instance_handles.erase(m_recycled_instance_handles.begin());
    
    return handle;
}

void vertex_batch::update_model()
{
    for (const auto &[handle, instance] : m_vertex_data_instances)
        m_VertexDataBuffer += instance;
    
    m_pModel->update_vertex_data(model::UsageHint::Streaming, 
        m_VertexDataBuffer);
    
    m_VertexDataBuffer.clear();
}

void vertex_batch::overwrite_or_add_instance(instance_handle_type &aHandle, const vertex_data &aVertexData)
{
    if (m_vertex_data_instances.find(aHandle) != m_vertex_data_instances.end())
    {
        m_vertex_data_instances.insert_or_assign(aHandle, aVertexData);

        return;
    }

    aHandle = add_instance(aVertexData);
}

void vertex_batch::remove_instance(const instance_handle_type aHandle)
{
    if (m_vertex_data_instances.erase(aHandle))
        m_recycled_instance_handles.insert(aHandle);
}

void vertex_batch::clear()
{
    m_VertexDataBuffer.clear();
    
    m_vertex_data_instances.clear();

    m_recycled_instance_handles.clear();
    
    m_handle_counter = 0;
}


//functor called when update_model is called, right before the final buffer gets uploaded
/*on_update_transform(std::function<void(byte *pData, size_t size)> transformer) //TODO: implement */

/*//! adds vertex data into the existing vertex data bufer
///
/// returns the offset into the buffer where the added vertex data starts
size_t add_vertex_data(const vertex_data &aVertexData);

//! overwrites existing vertex data with new data. returns true if it succeeded, false if it did not.
///
/// failure will occur if the new data would write past the buffer.
bool try_overwrite_vertex_data(const vertex_data &aVertexData, const size_t aOffset); //TODO maybe replace with overwrite_or_add

//void overwrite_vertex_data //throws if failure

//void overwrite_or_add //tries to overwrite, adds data to end if overwrite fails

//! erase all vertex data in the buffer
void clear_vertex_data();

//! [re]generates the model using vertex_data
void update_model();*/

/*bool vertex_batch::try_overwrite_vertex_data(const vertex_data &aVertexData, const size_t aOffset)
{
    if (aOffset % aVertexData.vertex_size()) return false;

    const auto &new_data(aVertexData.data());
    
    auto view_to_existing_data(m_VertexDataBuffer.view_to_interleaved_data());

    if (new_data.size() + aOffset > view_to_existing_data.size) return false;

    auto *p = view_to_existing_data.begin + aOffset;

    for (const auto data : new_data)
    {
        *p = data;

        p++;
    }

    return true;
}

size_t vertex_batch::add_vertex_data(const vertex_data &aVertexData)
{
    size_t index_of_instance = m_VertexDataBuffer.interleaved_data_size();

    m_VertexDataBuffer += aVertexData;

    return index_of_instance;
}

void vertex_batch::clear_vertex_data()
{
    m_VertexDataBuffer.clear();
}

void vertex_batch::update_model()
{
    m_pModel->update_vertex_data(model::UsageHint::Streaming,
        m_VertexDataBuffer);
}*/

