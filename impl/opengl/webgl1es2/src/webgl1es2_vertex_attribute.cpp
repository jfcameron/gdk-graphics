// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/webgl1es2_vertex_attribute.h>

using namespace gdk;

static constexpr char TAG[] = "vertex_attribute";

bool webgl1es2_vertex_attribute::operator==(const webgl1es2_vertex_attribute &that) const
{
    return 
        name == that.name &&
        size == that.size;
}

bool webgl1es2_vertex_attribute::operator!=(const webgl1es2_vertex_attribute &that) const
{
    return !(*this == that);
}

webgl1es2_vertex_attribute::webgl1es2_vertex_attribute(const std::string &aName, const unsigned short &aSize)
: name(aName)
, size(aSize)
{}
