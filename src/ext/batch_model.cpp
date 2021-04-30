// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics_context.h>
#include <gdk/ext/batch_model.h>

using namespace gdk;

batch_model::batch_model(std::shared_ptr<gdk::graphics::context> pContext, 
        std::vector<vertex_data> data)//TODO assert pos3 exists
    : m_pModel(pContext->make_model())
      , m_Inputs(data)
{}

std::shared_ptr<gdk::model> batch_model::model()
{
    return m_pModel;
}

void batch_model::write_to_buffer(size_t vertexDataIndex, 
    const graphics_vector3_type &aPos, 
    const graphics_vector3_type &aRot,
    const graphics_vector3_type &aScale)
{
    if (vertexDataIndex >= m_Inputs.size()) 
        throw std::invalid_argument("index out of range");

    auto newData = m_Inputs[vertexDataIndex];

    auto view = newData.view_to_interleaved_data();
    auto pos_offset = newData.attribute_offset("a_Position");
    auto vertex_size = newData.vertex_size();

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

    m_Buffer += std::move(newData);
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

