// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>
#include <gdk/vertex_format.h>
#include <gdk/shader_program.h>

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

vertex_format::vertex_format(const std::vector<vertex_attribute> &aAttributes)
: m_Format(aAttributes)
, m_SumOfAttributeComponents(([aAttributes]()
{
    decltype(m_SumOfAttributeComponents) sum(0);
    
    for (const auto &attribute : aAttributes) sum += attribute.size;
    
    return sum;
})())
{}

void vertex_format::enableAttributes(const shader_program &aShaderProgram) const
{
    int attributeOffset(0);
    
    for (const auto &attribute : m_Format)
    {
        std::string attributeName = attribute.name;
        int attributeSize = attribute.size;

        if (auto activeAttribute = aShaderProgram.tryGetActiveAttribute(attributeName); activeAttribute)
        {
            //if (attributeType == found->type && //TODO: support types, so not forced to use floats for small range datatypes
                //attributeSize == found->second.count) //TODO: count is not component count, its number of e.g vector2s
            //{
                glh::Enablevertex_attribute(activeAttribute->location, 
                    attributeSize, 
                    attributeOffset,
                    m_SumOfAttributeComponents);
            //}
        }
        
        attributeOffset += attributeSize;
    }
}

int vertex_format::getSumOfAttributeComponents() const
{
    return m_SumOfAttributeComponents;
}

