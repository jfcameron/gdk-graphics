// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>
#include <gdk/texture_uniform_collection.h>

using namespace gdk;

constexpr auto TAG = "textureuniform";

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
