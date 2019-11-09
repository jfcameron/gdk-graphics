// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/opengl.h>
#include <gdk/vertex_format.h>

#include <nlohmann/json.hpp>

#include <iostream>

using namespace gdk;

static constexpr char TAG[] = "vertex_format";

// Special values
vertex_format const vertex_format::Pos3uv2Norm3(
{
    {"a_Position",3},
    {"a_UV"      ,2},
    {"a_Normal"  ,3}
});

vertex_format const vertex_format::Pos3uv2(
{
    {"a_Position",3},
    {"a_UV"      ,2}
});

vertex_format const vertex_format::Pos3(
{
    {"a_Position",3}
});

std::ostream &gdk::operator<<(std::ostream &s, const vertex_format &a)
{
    (void)a;
    
    return s << nlohmann::json
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },
        
        {TAG, jfc::insertion_operator_to_nlohmann_json_object("Not Implemented!")},
    }
    .dump();
}

vertex_format::vertex_format(const std::vector<vertex_attribute> &aAttributes)
: m_Format(aAttributes)
, m_SumOfAttributeComponents(([aAttributes]() -> int
{
    int buf = 0;
    
    for (auto attribute : aAttributes) buf += attribute.size;
    
    return buf;
})())
{}

void vertex_format::enableAttributes(const GLuint ashader_programHandle) const
{
    int attributeOffset = 0;
    
    for(auto attribute : m_Format)
    {
        std::string attributeName = attribute.name;
        int attributeSize = attribute.size;
        
        glh::Enablevertex_attribute(
            attributeName, 
            ashader_programHandle, 
            attributeSize, 
            attributeOffset,
            m_SumOfAttributeComponents
        );
        
        attributeOffset += attributeSize;
    }
}

int vertex_format::getSumOfAttributeComponents() const
{
    return m_SumOfAttributeComponents;
}

