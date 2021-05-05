// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics_context.h>
#include <gdk/ext/batch_model.h>

using namespace gdk;

static void transform_vertex_data(vertex_data *const oNewData,
    const graphics_vector3_type &aPos, 
    const graphics_vector3_type &aRot,
    const graphics_vector3_type &aScale)
{
    auto view = oNewData->view_to_interleaved_data();
    auto pos_offset = oNewData->attribute_offset("a_Position");
    auto vertex_size = oNewData->vertex_size();

    auto *p = view.begin + pos_offset;

    auto rot_euler = aRot;

    for (size_t i(0); i < view.size; i+= vertex_size)
    {
        float *x = p + 0;
        float *y = p + 1;
        float *z = p + 2;

        // scale
        *x *= aScale.x;
        *y *= aScale.y;
        *z *= aScale.z;

        // rotate around x
        auto q = rot_euler.x;
        auto _x = *x, _y = *y, _z = *z;
        *y = (_y * std::cos(q)) - (_z * std::sin(q));
        *z = (_y * std::sin(q)) + (_z * std::cos(q));
        *x = _x;

        // rotate around y
        q = rot_euler.y;
        _x = *x, _y = *y, _z = *z;
        *x = (_x * std::cos(q)) - (_z * std::sin(q));
        *z = (_x * std::sin(q)) + (_z * std::cos(q));
        *y = _y;

        // rotate around z
        q = rot_euler.z;
        _x = *x, _y = *y, _z = *z;
        *x = (_x * std::cos(q)) - (_y * std::sin(q));
        *y = (_x * std::sin(q)) + (_y * std::cos(q));
        *z = _z;

        // translate
        *x += aPos.x;
        *y += aPos.y;
        *z += aPos.z;

        p += vertex_size;
    }
};

batch_model::batch_model(std::shared_ptr<gdk::graphics::context> pContext, 
    std::vector<vertex_data> data)//TODO assert pos3 exists
: m_pModel(pContext->make_model())
, m_Inputs(data)
{}

std::shared_ptr<gdk::model> batch_model::model()
{
    return m_pModel;
}

void batch_model::rewrite_buffer_at(size_t BufferIndex, 
    size_t vertexDataIndex,
    const graphics_vector3_type &aPos, 
    const graphics_vector3_type &aRot,
    const graphics_vector3_type &aScale)
{
    if (vertexDataIndex >= m_Inputs.size()) 
        throw std::invalid_argument("index out of range");

    auto newData(m_Inputs[vertexDataIndex]);

    transform_vertex_data(&newData, aPos, aRot, aScale);

    auto buffer_view = m_Buffer.view_to_interleaved_data();
    auto new_data_view = newData.view_to_interleaved_data();

    if (new_data_view.size > (buffer_view.size - BufferIndex))
        throw std::invalid_argument("rewrite would exceed buffer bounds");

    auto *pBuffer = buffer_view.begin + BufferIndex;
    for (size_t i(0); i < new_data_view.size; ++i)
    {
        *(pBuffer + i) = *(new_data_view.begin + i);
    }
}

size_t batch_model::write_to_buffer(size_t vertexDataIndex, 
    const graphics_vector3_type &aPos, 
    const graphics_vector3_type &aRot,
    const graphics_vector3_type &aScale)
{
    if (vertexDataIndex >= m_Inputs.size()) 
        throw std::invalid_argument("index out of range");

    auto newData(m_Inputs[vertexDataIndex]);

    transform_vertex_data(&newData, aPos, aRot, aScale);

    const auto indexToStartOfNewData = m_Buffer.interleaved_data_size() - 0;

    m_Buffer += std::move(newData);

    return indexToStartOfNewData;
}

void batch_model::clear_buffer()
{
    m_Buffer.clear();
}

void batch_model::update_model()
{
    m_pModel->update_vertex_data(model::UsageHint::Streaming,
        m_Buffer);
}

vertex_data::interleaved_data_view batch_model::view_to_interleaved_data()
{
    return m_Buffer.view_to_interleaved_data();
}

