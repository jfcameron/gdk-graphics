// © Joseph Cameron - All Rights Reserved

#include <gdk/vertex_data.h>
#include <stdexcept>
#include <iostream>

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

vertex_data::vertex_data(const attribute_data_type &aAttributeData)
{
    if (aAttributeData.size())
    {
        /*if (!aAttributeData.size()) throw std::invalid_argument("vertex_data: "
            "attribute data must contain at least one attribute data view");*/

        size_t vertexCount(aAttributeData.begin()->second.data_size() /
            aAttributeData.begin()->second.component_count());

        /*if (!vertexCount) throw std::invalid_argument("vertex_data: "
            "vertex attribute data must have data");*/

        for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
        {
            auto currentVertexCount = current_attribute_data_view.data_size() /
                current_attribute_data_view.component_count();

            if (currentVertexCount != vertexCount) throw std::invalid_argument("vertex_data: "
                "attribute data arrays must contribute to the same number of vertexes");
        }

        // Stores attribute data
        size_t offset(0); //TODO: do this
        for (const auto &[current_name, current_attribute_data_view] : aAttributeData)
        {
            size_t size = current_attribute_data_view.component_count();

            m_Format.push_back({current_name, size});

            m_AttributeOffsets[current_name] = offset;
            
            offset += size;
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
}

vertex_data::PrimitiveMode vertex_data::getPrimitiveMode() const { return m_PrimitiveMode; }

size_t vertex_data::interleaved_data_size() const
{
    return m_Data.size();
}

const std::vector<attribute_data_view::attribute_component_type> &vertex_data::getData() const 
    { return m_Data; }

const std::vector<std::pair<std::string, std::size_t>> &vertex_data::attribute_format() const 
    { return m_Format; }

size_t vertex_data::attribute_offset(const std::string &aName) const
{
    return m_AttributeOffsets.at(aName);
}

void vertex_data::operator+=(const vertex_data &&other) { push_back(std::move(other)); }

void vertex_data::push_back(const vertex_data &&other)
{
    if (other.m_Data.empty()) return;

    if (m_Data.empty()) 
    {   
        (*this) = other;

        return;
    }

    //TODO: format_is_not_compatible(a, b) -> static local, optional<exception>
    if (other.m_PrimitiveMode != m_PrimitiveMode) throw 
        std::invalid_argument("vertex_data: "
            "incoming primitive modes must match");

    if (other.m_Format != m_Format) throw 
        std::invalid_argument("vertex_data: "
            "incoming attribute format must match");
    //END
    
    m_Data.reserve(m_Data.size() + other.m_Data.size());
    m_Data.insert(m_Data.end(), other.m_Data.begin(), other.m_Data.end());
}

void vertex_data::clear()
{
    m_Data.clear();
    m_Format.clear();
    m_AttributeOffsets.clear();
}

std::vector<vertex_data::index_value_type> vertex_data::getIndexData() const 
{ return std::vector<vertex_data::index_value_type>(); }

vertex_data::interleaved_data_view vertex_data::view_to_interleaved_data()
{
    return {&m_Data[0], m_Data.size()};
}

size_t vertex_data::vertex_size() const
{
    size_t size(0);

    for (const auto &attrib : m_Format)
        size += attrib.second;

    return size;
}

//TODO: could memoize at asignment points
/*size_t vertex_data::offset_of_attribute_in_single_vertex(const std::string &aName)
{
    size_t offset(0);

    for (const auto &attrib : m_Format) 
        if (attrib.first == aName) break;
        else offset += attrib.second;

    return offset;
}*/

