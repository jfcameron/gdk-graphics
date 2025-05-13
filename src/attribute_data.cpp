// © Joseph Cameron - All Rights Reserved

#include <gdk/attribute_data.h>
#include <gdk/graphics_exception.h>

using namespace gdk;

size_t attribute_data::number_of_components_per_attribute() const { return m_NumberOfComponentsPerAttribute; }

size_t attribute_data::number_of_attributes_in_component_data() const {
    return m_Components.size() / number_of_components_per_attribute();
}

const std::vector<component_type> &attribute_data::components() const { return m_Components; }

std::vector<component_type> &attribute_data::components() { return m_Components; }

attribute_data::attribute_data(const std::vector<component_type> &aComponents, const size_t aComponentCount)
: m_Components(aComponents)
, m_NumberOfComponentsPerAttribute(aComponentCount)
{}

void attribute_data::push_back(const attribute_data &rhs) {
    m_Components.reserve(m_Components.size() + rhs.m_Components.size());
    m_Components.insert(m_Components.end(), rhs.m_Components.begin(), rhs.m_Components.end());
}

attribute_data &attribute_data::operator+=(const attribute_data &rhs) {
    push_back(rhs);
    return *this;
}

void attribute_data::overwrite(size_t vertexOffset, const attribute_data &other) {
    if (number_of_components_per_attribute() != other.number_of_components_per_attribute()) 
        throw graphics_exception("number of components per attribute must match on order to overwrite component data");

    const size_t componentOffset(vertexOffset * number_of_components_per_attribute());
    
    if (componentOffset + other.components().size() > components().size())
        throw graphics_exception("new attrib data would write past the end of current attribute data");

    std::copy(other.components().begin(), other.components().begin() + other.components().size(),
        components().begin() + componentOffset);
}

