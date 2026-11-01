// © Joseph Cameron - MIT License

#include <gdk/graphics/attribute_data.h>
#include <gdk/graphics/exception.h>

#include <string>

using namespace gdk;
using namespace gdk::graphics;

size_t attribute_data::number_of_components_per_attribute() const { return m_NumberOfComponentsPerAttribute; }

size_t attribute_data::number_of_attributes_in_component_data() const {
    if (!m_NumberOfComponentsPerAttribute) return 0;

    return m_Components.size() / m_NumberOfComponentsPerAttribute;
}

const std::vector<component_type> &attribute_data::components() const { return m_Components; }

std::vector<component_type> &attribute_data::components() { return m_Components; }

attribute_data::attribute_data(std::span<const component_type> aComponents, const size_t aComponentCount)
: m_NumberOfComponentsPerAttribute(aComponentCount)
, m_Components(aComponents.begin(), aComponents.end())
{}

attribute_data::attribute_data(std::initializer_list<component_type> aComponents, const size_t aComponentCount)
: m_NumberOfComponentsPerAttribute(aComponentCount)
, m_Components(aComponents)
{}

void attribute_data::push_back(const attribute_data &aRhs) {
    if (!m_NumberOfComponentsPerAttribute)
        m_NumberOfComponentsPerAttribute = aRhs.m_NumberOfComponentsPerAttribute;

    else if (aRhs.m_NumberOfComponentsPerAttribute
        && m_NumberOfComponentsPerAttribute != aRhs.m_NumberOfComponentsPerAttribute)
        throw exception(std::string("attribute_data::push_back: cannot append an attribute of ")
            .append(std::to_string(aRhs.m_NumberOfComponentsPerAttribute))
            .append(" components per attribute to one of ")
            .append(std::to_string(m_NumberOfComponentsPerAttribute))
            .append("; the result would not describe either"));

    m_Components.insert(m_Components.end(), aRhs.m_Components.begin(), aRhs.m_Components.end());
}

attribute_data &attribute_data::operator+=(const attribute_data &aRhs) {
    push_back(aRhs);
    return *this;
}

void attribute_data::overwrite(size_t aVertexOffset, const attribute_data &aOther) {
    if (number_of_components_per_attribute() != aOther.number_of_components_per_attribute()) 
        throw exception("number of components per attribute must match on order to overwrite component data");

    const size_t componentOffset(aVertexOffset * number_of_components_per_attribute());
    
    if (componentOffset + aOther.components().size() > components().size())
        throw exception("new attrib data would write past the end of current attribute data");

    std::copy(aOther.components().begin(), aOther.components().begin() + aOther.components().size(),
        components().begin() + componentOffset);
}

