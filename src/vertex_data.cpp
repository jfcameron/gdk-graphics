// © Joseph Cameron - All Rights Reserved

#include <gdk/vertex_data.h>
#include <stdexcept>

attribute_data_view::attribute_data_view(attribute_data_view::attribute_component_type *const pData, 
    const size_t aDataLength, 
    const size_t aComponentCount)
: m_pData(pData)
, m_DataLength(aDataLength)
, m_ComponentCount(aComponentCount)
{
    if (!aDataLength) throw std::invalid_argument("attribute data view must contain data");
    if (!aComponentCount) throw std::invalid_argument("attribute component count cannot be zero");
}

attribute_data_view::attribute_component_type *attribute_data_view::data_begin() const { return m_pData; }

size_t attribute_data_view::attribute_data_view::data_size() const { return m_DataLength; }

size_t attribute_data_view::component_count() const { return m_ComponentCount; }

vertex_data::vertex_data(const UsageHint aUsage, 
    const attribute_data_type &aAttributeData,
    std::vector<index_value_type> &&aIndexData)
: m_Usage(aUsage)
, m_Indicies(aIndexData)
{
    if (!aAttributeData.size()) throw std::invalid_argument("vertex_data: "
        "vertex data view must contain at least one attribute data view");

    size_t vertexCount(aAttributeData.begin()->second.data_size() /
        aAttributeData.begin()->second.component_count());

    if (!vertexCount) throw std::invalid_argument("vertex_data: "
        "vertex attribute data must have data");

    for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
    {
        auto currentVertexCount = current_attribute_data_view.data_size() /
            current_attribute_data_view.component_count();

        if (currentVertexCount != vertexCount) throw std::invalid_argument("vertex_data: "
            "attribute data arrays must contribute to the same number of vertexes");
    }

    // Stores attribute sizes
    for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
    {
        m_AttributeFormat.push_back({current_name, 
            current_attribute_data_view.component_count()});
    }

    // Interleaves the data
    const size_t vertexcount = aAttributeData.begin()->second.data_size() / 
        aAttributeData.begin()->second.component_count();

    for (size_t vertexcounter(0); vertexcounter < vertexcount; ++vertexcounter) 
    {
        for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
        {
            for (size_t i(0), s(current_attribute_data_view.component_count()); i < s; ++i)
            {
                m_Data.push_back(*(current_attribute_data_view.data_begin() + i + (vertexcounter * s)));
            }
        }
    }
}

vertex_data::PrimitiveMode vertex_data::getPrimitiveMode() const
{
    return m_PrimitiveMode;
}

vertex_data::UsageHint vertex_data::getUsageHint() const
{
    return m_Usage;
}

const std::vector<attribute_data_view::attribute_component_type> &vertex_data::getData() const
{
    return m_Data;
}

const std::vector<std::pair<std::string, size_t>> &vertex_data::getAttributeFormat() const
{
    return m_AttributeFormat;
}

const std::vector<vertex_data::index_value_type> &vertex_data::getIndexData() const
{
    return m_Indicies;
}

