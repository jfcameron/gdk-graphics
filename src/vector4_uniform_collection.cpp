// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>
#include <gdk/vector4.h>
#include <gdk/vector4_uniform_collection.h>

using namespace gdk;

static constexpr char TAG[] = "vector4_uniform_collection";

void vector4_uniform_collection::bind(const GLuint aProgramHandle) const
{
    for (auto &pair : m_Map) glh::Bind4FloatUniform(pair.first, *pair.second.get());
}

void vector4_uniform_collection::unbind(const GLuint aProgramHandle) const
{
    for (auto &pair : m_Map) glh::Bind4FloatUniform(pair.first, graphics_vector4_type::Zero);
}
