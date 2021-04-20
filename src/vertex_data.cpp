// © Joseph Cameron - All Rights Reserved

#include <gdk/vertex_data.h>
#include <stdexcept>

vertex_data::vertex_data(const UsageHint aUsage, const attribute_data_type &aAttributeData)
: m_Usage(aUsage)
{
    if (!aAttributeData.size()) throw std::invalid_argument("vertex_data: "
        "vertex data view must contain at least one attribute data view");

    size_t vertexCount(aAttributeData.begin()->second.m_DataLength /
        aAttributeData.begin()->second.m_ComponentCount);

    if (!vertexCount) throw std::invalid_argument("vertex_data: "
        "vertex attribute data must have data");

    for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
    {
        auto currentVertexCount = current_attribute_data_view.m_DataLength /
            current_attribute_data_view.m_ComponentCount;

        if (currentVertexCount != vertexCount) throw std::invalid_argument("vertex_data: "
            "attribute data arrays must contribute to the same number of vertexes");
    }

    // Stores attribute sizes
    for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
    {
        m_AttributeFormat.push_back({current_name, 
            current_attribute_data_view.m_ComponentCount});
    }

    // Interleaves the data
    const size_t vertexcount = aAttributeData.begin()->second.m_DataLength / 
        aAttributeData.begin()->second.m_ComponentCount;

    for (size_t vertexcounter(0); vertexcounter < vertexcount; ++vertexcounter) 
    {
        for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
        {
            for (size_t i(0), s(current_attribute_data_view.m_ComponentCount); i < s; ++i)
            {
                m_Data.push_back(*(current_attribute_data_view.m_pData + i + (vertexcounter * s)));
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

