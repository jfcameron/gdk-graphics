// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>
#include <gdk/texture_uniform_collection.h>
#include <gdk/nlohmann_json_util.h>

#include <nlohmann/json.hpp>

#include <iostream>

using namespace gdk;

constexpr auto TAG = "textureuniform";

std::ostream& gdk::operator<<(std::ostream& s, const texture_uniform_collection& a)
{
    nlohmann::json root = 
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },
    };

    for (auto &pair : a.m_Map) root[pair.first] = jfc::insertion_operator_to_nlohmann_json_object(pair.second.lock());

    return s << root.dump();
}

void texture_uniform_collection::bind(const GLuint aProgramHandle) const
{
    int i = 0;
    for (auto &pair : m_Map)
    {
        //gdk::log(TAG, "start a bind");

        auto texture = pair.second.lock();
        glh::BindtextureUniform(aProgramHandle, pair.first, texture->getHandle(), i++);

        //gdk::log(TAG, "end a bind");
    }
}

void texture_uniform_collection::unbind(const GLuint aProgramHandle) const
{
    int i = 0;    
    for (auto &pair : m_Map) glh::BindtextureUniform(aProgramHandle, pair.first, 0, i++);
}
