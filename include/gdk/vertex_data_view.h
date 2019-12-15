// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEX_DATA_VIEW_H
#define GDK_GFX_VERTEX_DATA_VIEW_H

#include <string>
#include <unordered_map>

class attribute_data_view
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

//! used to construct a model
//\warn a view does not own its data. The user must guarantee the data is valid for the lifetime of the view.
class vertex_data_view
{
public:
    enum class UsageHint
    {
        Static,
        Dynamic,
        Streaming
    };

    using attribute_data_type = std::unordered_map<std::string, attribute_data_view>;

//private:
    UsageHint m_Usage;

    attribute_data_type m_AttributeData;

public:
    vertex_data_view(const UsageHint aUsage, const attribute_data_type &aAttributeData)
    : m_Usage(aUsage)
    , m_AttributeData(aAttributeData)
    {}
};

#endif
