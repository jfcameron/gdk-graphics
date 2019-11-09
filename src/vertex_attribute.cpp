// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/nlohmann_json_util.h>
#include <gdk/vertex_attribute.h>

#include <nlohmann/json.hpp>

#include <iostream>

using namespace gdk;

static constexpr char TAG[] = "vertex_attribute";

std::ostream &gdk::operator<<(std::ostream &s, const vertex_attribute &a)
{
    return s << nlohmann::json
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },
        
        {"name", jfc::insertion_operator_to_nlohmann_json_object(a.name)},
        {"size", jfc::insertion_operator_to_nlohmann_json_object(a.size)},
    }
    .dump();
}

vertex_attribute::vertex_attribute(const std::string &aName, const unsigned short &aSize)
: name(aName)
, size(aSize)
{}
