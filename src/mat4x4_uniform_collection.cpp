// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/mat4x4_uniform_collection.h>
#include <gdk/opengl.h>

static constexpr char TAG[] = "mat4x4_uniform_collection";

using namespace gdk;
    
void mat4x4_uniform_collection::bind(const gdk::shader_program &aProgram) const
{
    for (auto &pair : m_Map) glh::BindMatrix4x4(pair.first, pair.second);
}

void mat4x4_uniform_collection::unbind(const gdk::shader_program &aProgram) const
{
    for (auto &pair : m_Map) glh::BindMatrix4x4(pair.first, graphics_mat4x4_type::Identity);    
}
