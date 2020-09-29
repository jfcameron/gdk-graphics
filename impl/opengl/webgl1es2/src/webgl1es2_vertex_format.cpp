// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>
#include <gdk/webgl1es2_vertex_format.h>
#include <gdk/webgl1es2_shader_program.h>

using namespace gdk;

static constexpr char TAG[] = "vertex_format";

// Special values
webgl1es2_vertex_format const webgl1es2_vertex_format::Pos3uv2Norm3(
{
    {"a_Position",3},
    {"a_UV"      ,2},
    {"a_Normal"  ,3}
});

webgl1es2_vertex_format const webgl1es2_vertex_format::Pos3uv2(
{
    {"a_Position",3},
    {"a_UV"      ,2}
});

webgl1es2_vertex_format const webgl1es2_vertex_format::Pos3(
{
    {"a_Position",3}
});

webgl1es2_vertex_format::webgl1es2_vertex_format(const std::vector<webgl1es2_vertex_attribute> &aAttributes)
: m_Format(aAttributes)
, m_SumOfAttributeComponents(([aAttributes]()
{
    decltype(m_SumOfAttributeComponents) sum(0);
    
    for (const auto &attribute : aAttributes) sum += attribute.size;
    
    return sum;
})())
{}

void webgl1es2_vertex_format::enableAttributes(const webgl1es2_shader_program &aShaderProgram) const
{
    int attributeOffset(0);
    
    for (const auto &attribute : m_Format)
    {
        std::string attributeName = attribute.name;
        int attributeSize = attribute.size;

        if (auto activeAttribute = aShaderProgram.tryGetActiveAttribute(attributeName); activeAttribute)
        {
			glh::Enablevertex_attribute(activeAttribute->location, 
				attributeSize, 
                attributeOffset,
                m_SumOfAttributeComponents);
        }
        
        attributeOffset += attributeSize;
    }
}

int webgl1es2_vertex_format::getSumOfAttributeComponents() const
{
    return m_SumOfAttributeComponents;
}
