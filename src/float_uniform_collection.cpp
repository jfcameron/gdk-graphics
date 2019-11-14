// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/float_uniform_collection.h>
#include <gdk/glh.h>

static constexpr char TAG[] = "float_uniform_collection";

using namespace gdk;

void float_uniform_collection::bind(const GLuint aProgramHandle) const
{
    for (auto &pair : m_Map) glh::Bind1FloatUniform(aProgramHandle, pair.first, *pair.second.get());
}

void float_uniform_collection::unbind(const GLuint aProgramHandle) const
{
    for (auto &pair : m_Map) glh::Bind1FloatUniform(aProgramHandle, pair.first, 0);
}

