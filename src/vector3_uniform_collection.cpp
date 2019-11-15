// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>
#include <gdk/vector3_uniform_collection.h>

using namespace gdk;

static constexpr char TAG[] = "vector3_uniform_collection";

void vector3_uniform_collection::bind(const GLuint aProgramHandle) const
{
    for (auto &pair : m_Map) glh::Bind3FloatUniform(pair.first, *pair.second.get());
}

void vector3_uniform_collection::unbind(const GLuint aProgramHandle) const
{
    for (auto &pair : m_Map) glh::Bind3FloatUniform(pair.first, graphics_vector3_type::Zero);
}
