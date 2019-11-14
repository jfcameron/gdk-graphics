#include <gdk/float_uniform_collection.h>
#include <gdk/glh.h>
#include <gdk/uniform_manager.h>

using namespace gdk;

void uniform_manager::bind(const GLuint aProgramHandle) const
{
    for (const auto &uniform : m_FloatUniforms)
    {
        glh::Bind1FloatUniform(aProgramHandle, uniform.first, uniform.second);
    }
}
