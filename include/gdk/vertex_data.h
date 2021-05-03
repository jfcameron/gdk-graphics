// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEX_DATA_VIEW_H
#define GDK_GFX_VERTEX_DATA_VIEW_H

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/// \brief a view on to the data for a given attribute e.g: "uv", "position".
/// \warn a view does not own its data, the user must guarantee the data lives 
/// as long as the view could be read from
class attribute_data_view final
{
public:
    //! all vertex components must be of this type.
    using attribute_component_type = float; 

    attribute_data_view(attribute_component_type *const pData, 
        const size_t aDataLength, 
        const size_t aComponentCount);

    attribute_component_type *data_begin() const;
    
    size_t data_size() const;
    
    size_t component_count() const;

private:
    //! ptr to the beginning of the data
    /// \warn unowning
    attribute_component_type *m_pData; 

    //! total number of components in the data
    size_t m_DataLength; 

    //! number of components in a single attribute
    size_t m_ComponentCount; 
};

//! used to construct a model. Vertex data represents a set of vertex data in system memory
class vertex_data final
{
public:
    using index_value_type = unsigned short;

    enum class PrimitiveMode
    {
        Triangles
    };

    using attribute_data_type = 
        std::unordered_map<std::string, attribute_data_view>;
    
    struct interleaved_data_view
    {
        using value_type = attribute_data_view::attribute_component_type;

        value_type *const begin;

        const std::size_t size;
    };

    PrimitiveMode getPrimitiveMode() const;

    const std::vector<attribute_data_view::attribute_component_type> &getData() const;

    const std::vector<std::pair<std::string, std::size_t>> &attribute_format() const;
    
    size_t attribute_offset(const std::string &aName) const;

    //! append a different vertex_data to this vertex_data
    /// \warn must be same format
    void operator+=(const vertex_data &&other);
    //! append a different vertex_data to this vertex_data
    /// \warn must be same format
    void push_back(const vertex_data &&other);

    //! clears all state from this vertex_data instance
    void clear();
    
    vertex_data(const attribute_data_type &aAttributeData);

    interleaved_data_view view_to_interleaved_data();

    size_t vertex_size() const;

    size_t interleaved_data_size() const;

    //TODO: this returns a constant, need index support
    std::vector<index_value_type> getIndexData() const;

private:
    //! primitive type to emit at primitive stage TODO: support other modes
    PrimitiveMode m_PrimitiveMode = PrimitiveMode::Triangles; 

    //! Raw data, interleaved
    std::vector<attribute_data_view::attribute_component_type> m_Data;

    //! Attribute format
    std::vector<std::pair<std::string, size_t>> m_Format;
    
    std::unordered_map<std::string, size_t> m_AttributeOffsets;
   
    //! Indicies improve draw performance by reducing the number 
    /// of unique verticies, however they are not required.
    //TODO: support indicies
    //std::vector<index_value_type> m_Indicies = {};
};

#endif

