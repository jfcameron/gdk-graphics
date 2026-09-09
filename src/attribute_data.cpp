// © Joseph Cameron - All Rights Reserved

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

void attribute_data::push_back(const attribute_data &rhs) {
    if (!m_NumberOfComponentsPerAttribute)
        m_NumberOfComponentsPerAttribute = rhs.m_NumberOfComponentsPerAttribute;

    else if (rhs.m_NumberOfComponentsPerAttribute
        && m_NumberOfComponentsPerAttribute != rhs.m_NumberOfComponentsPerAttribute)
        throw exception(std::string("attribute_data::push_back: cannot append an attribute of ")
            .append(std::to_string(rhs.m_NumberOfComponentsPerAttribute))
            .append(" components per attribute to one of ")
            .append(std::to_string(m_NumberOfComponentsPerAttribute))
            .append("; the result would not describe either"));

    m_Components.reserve(m_Components.size() + rhs.m_Components.size());
    m_Components.insert(m_Components.end(), rhs.m_Components.begin(), rhs.m_Components.end());
}

attribute_data &attribute_data::operator+=(const attribute_data &rhs) {
    push_back(rhs);
    return *this;
}

void attribute_data::overwrite(size_t vertexOffset, const attribute_data &other) {
    if (number_of_components_per_attribute() != other.number_of_components_per_attribute()) 
        throw exception("number of components per attribute must match on order to overwrite component data");

    const size_t componentOffset(vertexOffset * number_of_components_per_attribute());
    
    if (componentOffset + other.components().size() > components().size())
        throw exception("new attrib data would write past the end of current attribute data");

    std::copy(other.components().begin(), other.components().begin() + other.components().size(),
        components().begin() + componentOffset);
}

