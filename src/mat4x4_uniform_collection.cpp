// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/mat4x4_uniform_collection.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/opengl.h>

#include <nlohmann/json.hpp>

#include <iostream>

static constexpr char TAG[] = "mat4x4_uniform_collection";

namespace gdk
{
    std::ostream &operator<<(std::ostream &s, const mat4x4_uniform_collection &a)
    {
        nlohmann::json root = 
        {
            {"Type", TAG}, 
            {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
                {}
            },
        };

        //for (auto &pair : a.m_Map) root[pair.first] = jfc::insertion_operator_to_nlohmann_json_object(pair.second);

        return s << root.dump();
    }

    void mat4x4_uniform_collection::bind(const GLuint aProgramHandle) const
    {
        for (auto &pair : m_Map) glh::BindMatrix4x4(aProgramHandle, pair.first, pair.second);
    }

    void mat4x4_uniform_collection::unbind(const GLuint aProgramHandle) const
    {
        for (auto &pair : m_Map) glh::BindMatrix4x4(aProgramHandle, pair.first, graphics_mat4x4_type::Identity);    
    }
}
