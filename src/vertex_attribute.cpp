// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/vertex_attribute.h>

#include <nlohmann/json.hpp>

using namespace gdk;

static constexpr char TAG[] = "vertex_attribute";

bool vertex_attribute::operator==(const vertex_attribute &b) const
{
    return 
        name == b.name &&
        size == b.size;
}

bool vertex_attribute::operator!=(const vertex_attribute &b) const
{
    return !(*this == b);
}

vertex_attribute::vertex_attribute(const std::string &aName, const unsigned short &aSize)
: name(aName)
, size(aSize)
{}
