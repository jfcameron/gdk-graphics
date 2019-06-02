// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/floatuniformcollection.h>
#include <gdk/glh.h>
#include <gdk/nlohmann_json_util.h>

#include <nlohmann/json.hpp>

#include <iostream>

static constexpr char TAG[] = "FloatUniformCollection";

namespace gdk
{
std::ostream &operator<<(std::ostream &s, const FloatUniformCollection &a)
{
    nlohmann::json root = 
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },
    };

    for (auto &pair : a.m_Map) root[pair.first] = *pair.second.get();

    return s << root.dump();
}

void FloatUniformCollection::bind(const GLuint aProgramHandle) const
{
    for (auto &pair : m_Map) glh::Bind1FloatUniform(aProgramHandle, pair.first, *pair.second.get());
}

void FloatUniformCollection::unbind(const GLuint aProgramHandle) const
{
    for (auto &pair : m_Map) glh::Bind1FloatUniform(aProgramHandle, pair.first, 0);
}
}
