// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEX_DATA_VIEW_H
#define GDK_GFX_VERTEX_DATA_VIEW_H

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <vector>

/// \brief a view on to the data for a given attribute e.g: "uv", "position".
/// \warn a view does not own its data, the user must guarantee the data lives as long as the view could be read from
class attribute_data_view final
{
public:
    //! all vertex components must be of this type.
    /// TODO: consider supporting additional types.Reality is if only GLES2 is supported, all attrib types are float based so all components will be float32s.
    /// if additional implementations are ever supported, then this could be a good way to reduce vram usage.
    using attribute_component_type = float; //TODO: gl guarantees GLfloat has a with of 32bits. c has stdlib int type widths. is there float?

//private:
    //! ptr to the beginning of the data
    /// \warn unowning
    attribute_component_type *m_pData; 

    //! total number of components in the data
    size_t m_DataLength; 

    //! number of components in a single attribute
    size_t m_ComponentCount; 

public:
    attribute_data_view(attribute_component_type *pData, size_t aDataLength, size_t aComponentCount)
    : m_pData(pData)
    , m_DataLength(aDataLength)
    , m_ComponentCount(aComponentCount)
    {
        if (!aDataLength) throw std::invalid_argument("attribute data view must contain data");
        if (!aComponentCount) throw std::invalid_argument("attribute component count cannot be zero");
    }
};

//! used to construct a model. Vertex data represents a set of vertex data in system memory
class vertex_data final
{
public:
    enum class UsageHint
    {
        Static,
        Dynamic,
        Streaming
    };

    enum class PrimitiveMode
    {
        Triangles
    };

    using attribute_data_type = std::unordered_map<std::string, attribute_data_view>;

    vertex_data(const UsageHint aUsage, const attribute_data_type &aAttributeData);

    PrimitiveMode getPrimitiveMode() const;

    UsageHint getUsageHint() const;

    const std::vector<attribute_data_view::attribute_component_type> &getData() const;

    const std::vector<std::pair<std::string, size_t>> &getAttributeFormat() const;

private:
    //! primitive type to emit at primitive stage
    PrimitiveMode m_PrimitiveMode = PrimitiveMode::Triangles; 

    //! the context may use this to optimize where in video memory the data is uploaded
    UsageHint m_Usage;

    //! Raw data, interleaved
    std::vector<attribute_data_view::attribute_component_type> m_Data;

    //! Attribute format
    std::vector<std::pair<std::string, size_t>> m_AttributeFormat;

};

#endif
